@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S "%0" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
goto endofperl
@rem ';
#!perl -w
#line 14

#line 18

=head1 NAME

lwp-rget - Retrieve WWW documents recursively

=head1 SYNOPSIS

 lwp-rget [--verbose] [--auth=USER:PASS] [--depth=N] [--hier] [--iis]
	  [--keepext=mime/type[,mime/type]] [--limit=N] [--nospace]
	  [--prefix=URL] [--referer=URL] [--sleep=N] [--tolower] <URL>
 lwp-rget --version

=head1 DESCRIPTION

This program will retrieve a document and store it in a local file.  It
will follow any links found in the document and store these documents
as well, patching links so that they refer to these local copies.
This process continues until there are no more unvisited links or the
process is stopped by the one or more of the limits which can be
controlled by the command line arguments.

This program is useful if you want to make a local copy of a
collection of documents or want to do web reading off-line.

All documents are stored as plain files in the current directory. The
file names chosen are derived from the last component of URL paths.

The options are:

=over 3

=item --auth=USER:PASS<n>

Set the authentication credentials to user "USER" and password "PASS" if
any restricted parts of the web site are hit.  If there are restricted
parts of the web site and authentication credentials are not available,
those pages will not be downloaded.

=item --depth=I<n>

Limit the recursive level. Embedded images are always loaded, even if
they fall outside the I<--depth>. This means that one can use
I<--depth=0> in order to fetch a single document together with all
inline graphics.

The default depth is 5.

=item --hier

Download files into a hierarchy that mimics the web site structure.
The default is to put all files in the current directory.

=item --referer=I<URI>

Set the value of the referer header for the initial request.  The
special value C<"NONE"> can be used to suppress the referer header in
any of subsequent requests.

=item --iis

Sends an "Accept: */*" on all URL requests as a workaround for a bug in
IIS 2.0.  If no Accept MIME header is present, IIS 2.0 returns with a
"406 No acceptable objects were found" error.  Also converts any back
slashes (\\) in URLs to forward slashes (/).

=item --keepext=I<mime/type[,mime/type]>

Keeps the current extension for the list MIME types.  Useful when
downloading text/plain documents that shouldn't all be translated to
*.txt files.

=item --limit=I<n>

Limit the number of documents to get.  The default limit is 50.

=item --nospace

Changes spaces in all URLs to underscore characters (_).  Useful when
downloading files from sites serving URLs with spaces in them.	Does not
remove spaces from fragments, e.g., "file.html#somewhere in here".

=item --prefix=I<url_prefix>

Limit the links to follow. Only URLs that start the prefix string are
followed.

The default prefix is set as the "directory" of the initial URL to
follow.	 For instance if we start lwp-rget with the URL
C<http://www.sn.no/foo/bar.html>, then prefix will be set to
C<http://www.sn.no/foo/>.

Use C<--prefix=''> if you don't want the fetching to be limited by any
prefix.

=item --sleep=I<n>

Sleep I<n> seconds before retrieving each document. This options allows
you to go slowly, not loading the server you visiting too much.

=item --tolower

Translates all links to lowercase.  Useful when downloading files from
IIS since it does not serve files in a case sensitive manner.

=item --verbose

Make more noise while running.

=item --quiet

Don't make any noise.

=item --version

Print program version number and quit.

=item --help

Print the usage message and quit.

=back

Before the program exits the name of the file, where the initial URL
is stored, is printed on stdout.  All used filenames are also printed
on stderr as they are loaded.  This printing can be suppressed with
the I<--quiet> option.

=head1 SEE ALSO

L<lwp-request>, L<LWP>

=head1 AUTHOR

Gisle Aas <aas@sn.no>

=cut

use strict;

use Getopt::Long    qw(GetOptions);
use URI::URL	    qw(url);
use LWP::MediaTypes qw(media_suffix);
use HTML::Entities  ();

use vars qw($VERSION);
use vars qw($MAX_DEPTH $MAX_DOCS $PREFIX $REFERER $VERBOSE $QUIET $SLEEP $HIER $AUTH $IIS $TOLOWER $NOSPACE %KEEPEXT);

my $progname = $0;
$progname =~ s|.*/||;  # only basename left
$progname =~ s/\.\w*$//; #strip extension if any

$VERSION = sprintf("%d.%02d", q$Revision: 1.19 $ =~ /(\d+)\.(\d+)/);

#$Getopt::Long::debug = 1;
#$Getopt::Long::ignorecase = 0;

# Defaults
$MAX_DEPTH = 5;
$MAX_DOCS  = 50;

GetOptions('version'  => \&print_version,
	   'help'     => \&usage,
	   'depth=i'  => \$MAX_DEPTH,
	   'limit=i'  => \$MAX_DOCS,
	   'verbose!' => \$VERBOSE,
	   'quiet!'   => \$QUIET,
	   'sleep=i'  => \$SLEEP,
	   'prefix:s' => \$PREFIX,
	   'referer:s'=> \$REFERER,
	   'hier'     => \$HIER,
	   'auth=s'   => \$AUTH,
	   'iis'      => \$IIS,
	   'tolower'  => \$TOLOWER,
	   'nospace'  => \$NOSPACE,
	   'keepext=s' => \$KEEPEXT{'OPT'},
	  ) || usage();

sub print_version {
    require LWP;
    my $DISTNAME = 'libwww-perl-' . LWP::Version();
    print <<"EOT";
This is lwp-rget version $VERSION ($DISTNAME)

Copyright 1996-1998, Gisle Aas.

This program is free software; you can redistribute it and/or
modify it under the same terms as Perl itself.
EOT
    exit 0;
}

my $start_url = shift || usage();
usage() if @ARGV;

require LWP::UserAgent;
my $ua = new LWP::UserAgent;
$ua->agent("$progname/$VERSION " . $ua->agent);
$ua->env_proxy;

unless (defined $PREFIX) {
    $PREFIX = url($start_url);	 # limit to URLs below this one
    eval {
	$PREFIX->eparams(undef);
	$PREFIX->equery(undef);
    };

    $_ = $PREFIX->epath;
    s|[^/]+$||;
    $PREFIX->epath($_);
    $PREFIX = $PREFIX->as_string;
}

%KEEPEXT = map { lc($_) => 1 } split(/\s*,\s*/, ($KEEPEXT{'OPT'}||0));

my $SUPPRESS_REFERER;
$SUPPRESS_REFERER++ if ($REFERER || "") eq "NONE";

print <<"" if $VERBOSE;
START	  = $start_url
MAX_DEPTH = $MAX_DEPTH
MAX_DOCS  = $MAX_DOCS
PREFIX	  = $PREFIX

my $no_docs = 0;
my %seen = ();	   # mapping from URL => local_file

my $filename = fetch($start_url, undef, $REFERER);
print "$filename\n" unless $QUIET;

sub fetch
{
    my($url, $type, $referer, $depth) = @_;

    # Fix http://sitename.com/../blah/blah.html to
    #	  http://sitename.com/blah/blah.html
    $url = $url->as_string if (ref($url));
    while ($url =~ s#(https?://[^/]+/)\.\.\/#$1#) {}

    # Fix backslashes (\) in URL if $IIS defined
    $url = fix_backslashes($url) if (defined $IIS);

    $url = url($url) unless ref($url);
    $type  ||= 'a';
    # Might be the background attribute
    $type = 'img' if ($type eq 'body' || $type eq 'td');
    $depth ||= 0;

    # Print the URL before we start checking...
    my $out = (" " x $depth) . $url . " ";
    $out .= "." x (60 - length($out));
    print STDERR $out . " " if $VERBOSE;

    # Can't get mailto things
    if ($url->scheme eq 'mailto') {
	print STDERR "*skipping mailto*\n" if $VERBOSE;
	return $url->as_string;
    }

    # The $plain_url is a URL without the fragment part
    my $plain_url = $url->clone;
    $plain_url->frag(undef);

    # Check PREFIX, but not for <IMG ...> links
    if ($type ne 'img' and  $url->as_string !~ /^\Q$PREFIX/o) {
	print STDERR "*outsider*\n" if $VERBOSE;
	return $url->as_string;
    }

    # Translate URL to lowercase if $TOLOWER defined
    $plain_url = to_lower($plain_url) if (defined $TOLOWER);

    # If we already have it, then there is nothing to be done
    my $seen = $seen{$plain_url->as_string};
    if ($seen) {
	my $frag = $url->frag;
	$seen .= "#$frag" if defined($frag);
	$seen = protect_frag_spaces($seen);
	print STDERR "$seen (again)\n" if $VERBOSE;
	return $seen;
    }

    # Too much or too deep
    if ($depth > $MAX_DEPTH and $type ne 'img') {
	print STDERR "*too deep*\n" if $VERBOSE;
	return $url;
    }
    if ($no_docs > $MAX_DOCS) {
	print STDERR "*too many*\n" if $VERBOSE;
	return $url;
    }

    # Fetch document 
    $no_docs++;
    sleep($SLEEP) if $SLEEP;
    my $req = HTTP::Request->new(GET => $url);
    # See: http://ftp.sunet.se/pub/NT/mirror-microsoft/kb/Q163/7/74.TXT
    $req->header ('Accept', '*/*') if (defined $IIS);  # GIF/JPG from IIS 2.0
    $req->authorization_basic(split (/:/, $AUTH)) if (defined $AUTH);
    $req->referer($referer) if $referer && !$SUPPRESS_REFERER;
    my $res = $ua->request($req);

    # Check outcome
    if ($res->is_success) {
	my $doc = $res->content;
	my $ct = $res->content_type;
	my $name = find_name($res->request->url, $ct);
	print STDERR "$name\n" unless $QUIET;
	$seen{$plain_url->as_string} = $name;

	# If the file is HTML, then we look for internal links
	if ($ct eq "text/html") {
	    # Save an unprosessed version of the HTML document.	 This
	    # both reserves the name used, and it also ensures that we
	    # don't loose everything if this program is killed before
	    # we finish.
	    save($name, $doc);
	    my $base = $res->base;

	    # Follow and substitute links...
	    $doc =~
s/
  (
    <(img|a|body|area|frame|td)\b   # some interesting tag
    [^>]+			    # still inside tag (not strictly correct)
    \b(?:src|href|background)	    # some link attribute
    \s*=\s*			    # =
  )
    (?:				    # scope of OR-ing
	 (")([^"]*)"	|	    # value in double quotes  OR
	 (')([^']*)'	|	    # value in single quotes  OR
	    ([^\s>]+)		    # quoteless value
    )
/
  new_link($1, lc($2), $3||$5, HTML::Entities::decode($4||$6||$7),
           $base, $name, "$url", $depth+1)
/giex;
	   # XXX
	   # The regular expression above is not strictly correct.
	   # It is not really possible to parse HTML with a single
	   # regular expression, but it is faster.  Tags that might
	   # confuse us include:
	   #	<a alt="href" href=link.html>
	   #	<a alt=">" href="link.html">
	   #
	}
	save($name, $doc);
	return $name;
    } else {
	print STDERR $res->code . " " . $res->message . "\n" if $VERBOSE;
	$seen{$plain_url->as_string} = $url->as_string;
	return $url->as_string;
    }
}

sub new_link
{
    my($pre, $type, $quote, $url, $base, $localbase, $referer, $depth) = @_;

    $url = protect_frag_spaces($url);

    $url = fetch(url($url, $base)->abs, $type, $referer, $depth);
    $url = url("file:$url", "file:$localbase")->rel
	unless $url =~ /^[.+\-\w]+:/;

    $url = unprotect_frag_spaces($url);

    return $pre . $quote . $url . $quote;
}


sub protect_frag_spaces
{
    my ($url) = @_;

    $url = $url->as_string if (ref($url));

    if ($url =~ m/^([^#]*#)(.+)$/)
    {
      my ($base, $frag) = ($1, $2);
      $frag =~ s/ /%20/g;
      $url = $base . $frag;
    }

    return $url;
}


sub unprotect_frag_spaces
{
    my ($url) = @_;

    $url = $url->as_string if (ref($url));

    if ($url =~ m/^([^#]*#)(.+)$/)
    {
      my ($base, $frag) = ($1, $2);
      $frag =~ s/%20/ /g;
      $url = $base . $frag;
    }

    return $url;
}


sub fix_backslashes
{
    my ($url) = @_;
    my ($base, $frag);

    $url = $url->as_string if (ref($url));

    if ($url =~ m/([^#]+)(#.*)/)
    {
      ($base, $frag) = ($1, $2);
    }
    else
    {
      $base = $url;
      $frag = "";
    }

    $base =~ tr/\\/\//;
    $base =~ s/%5[cC]/\//g;	# URL-encoded back slash is %5C

    return $base . $frag;
}


sub to_lower
{
    my ($url) = @_;
    my $was_object = 0;

    if (ref($url))
    {
      $url = $url->as_string;
      $was_object = 1;
    }

    if ($url =~ m/([^#]+)(#.*)/)
    {
      $url = lc($1) . $2;
    }
    else
    {
      $url = lc($url);
    }

    if ($was_object == 1)
    {
      return url($url);
    }
    else
    {
      return $url;
    }
}


sub translate_spaces
{
    my ($url) = @_;
    my ($base, $frag);

    $url = $url->as_string if (ref($url));

    if ($url =~ m/([^#]+)(#.*)/)
    {
      ($base, $frag) = ($1, $2);
    }
    else
    {
      $base = $url;
      $frag = "";
    }

    $base =~ s/^ *//;	# Remove initial spaces from base
    $base =~ s/ *$//;	# Remove trailing spaces from base

    $base =~ tr/ /_/;
    $base =~ s/%20/_/g; # URL-encoded space is %20

    return $base . $frag;
}


sub mkdirp
{
    my($directory, $mode) = @_;
    my @dirs = split(/\//, $directory);
    my $path = shift(@dirs);   # build it as we go
    my $result = 1;   # assume it will work

    unless (-d $path) {
	$result &&= mkdir($path, $mode);
    }

    foreach (@dirs) {
	$path .= "/$_";
	if ( ! -d $path) {
	    $result &&= mkdir($path, $mode);
	}
    }

    return $result;
}


sub find_name
{
    my($url, $type) = @_;
    #print "find_name($url, $type)\n";

    # Translate spaces in URL to underscores (_) if $NOSPACE defined
    $url = translate_spaces($url) if (defined $NOSPACE);

    # Translate URL to lowercase if $TOLOWER defined
    $url = to_lower($url) if (defined $TOLOWER);

    $url = url($url) unless ref($url);

    my $path = $url->path;

    # trim path until only the basename is left
    $path =~ s|(.*/)||;
    my $dirname = ".$1";
    if (!$HIER) {
	$dirname = "";
    } elsif (! -d $dirname) {
	mkdirp($dirname, 0775);
    }

    my $extra = "";  # something to make the name unique
    my $suffix;

    if ($KEEPEXT{lc($type)}) {
        $suffix = ($path =~ m/\.(.*)/) ? $1 : "";
    } else {
        $suffix = media_suffix($type);
    }

    $path =~ s|\..*||;	# trim suffix
    $path = "index" unless length $path;

    while (1) {
	# Construct a new file name
	my $file = $dirname . $path . $extra;
	$file .= ".$suffix" if $suffix;
	# Check if it is unique
	return $file unless -f $file;

	# Try something extra
	unless ($extra) {
	    $extra = "001";
	    next;
	}
	$extra++;
    }
}


sub save
{
    my $name = shift;
    #print "save($name,...)\n";
    open(FILE, ">$name") || die "Can't save $name: $!";
    binmode FILE;
    print FILE $_[0];
    close(FILE);
}


sub usage
{
    die <<"";
Usage: $progname [options] <URL>
Allowed options are:
  --auth=USER:PASS  Set authentication credentials for web site
  --depth=N	    Maximum depth to traverse (default is $MAX_DEPTH)
  --hier	    Download into hierarchy (not all files into cwd)
  --referer=URI     Set initial referer header (or "NONE")
  --iis		    Workaround IIS 2.0 bug by sending "Accept: */*" MIME
		    header; translates backslashes (\\) to forward slashes (/)
  --keepext=type    Keep file extension for MIME types (comma-separated list)
  --limit=N	    A limit on the number documents to get (default is $MAX_DOCS)
  --nospace	    Translate spaces URLs (not #fragments) to underscores (_)
  --version	    Print version number and quit
  --verbose	    More output
  --quiet	    No output
  --sleep=SECS	    Sleep between gets, ie. go slowly
  --prefix=PREFIX   Limit URLs to follow to those which begin with PREFIX
  --tolower	    Translate all URLs to lowercase (useful with IIS servers)

}
__END__
:endofperl
