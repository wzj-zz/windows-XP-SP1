# $Id: Daemon.pm,v 1.21 1999/03/20 07:37:35 gisle Exp $
#

use strict;

package HTTP::Daemon;

=head1 NAME

HTTP::Daemon - a simple http server class

=head1 SYNOPSIS

  use HTTP::Daemon;
  use HTTP::Status;

  my $d = new HTTP::Daemon;
  print "Please contact me at: <URL:", $d->url, ">\n";
  while (my $c = $d->accept) {
      while (my $r = $c->get_request) {
	  if ($r->method eq 'GET' and $r->url->path eq "/xyzzy") {
              # remember, this is *not* recommened practice :-)
	      $c->send_file_response("/etc/passwd");
	  } else {
	      $c->send_error(RC_FORBIDDEN)
	  }
      }
      $c->close;
      undef($c);
  }

=head1 DESCRIPTION

Instances of the I<HTTP::Daemon> class are HTTP/1.1 servers that
listen on a socket for incoming requests. The I<HTTP::Daemon> is a
sub-class of I<IO::Socket::INET>, so you can perform socket operations
directly on it too.

The accept() method will return when a connection from a client is
available. The returned value will be a reference to a object of the
I<HTTP::Daemon::ClientConn> class which is another I<IO::Socket::INET>
subclass. Calling the get_request() method on this object will read
data from the client and return an I<HTTP::Request> object reference.

This HTTP daemon does not fork(2) for you.  Your application, i.e. the
user of the I<HTTP::Daemon> is reponsible for forking if that is
desirable.  Also note that the user is responsible for generating
responses that conform to the HTTP/1.1 protocol.  The
I<HTTP::Daemon::ClientConn> class provides some methods that make this easier.

=head1 METHODS

The following is a list of methods that are new (or enhanced) relative
to the I<IO::Socket::INET> base class.

=over 4

=cut


use vars qw($VERSION @ISA $PROTO $DEBUG);

$VERSION = sprintf("%d.%02d", q$Revision: 1.21 $ =~ /(\d+)\.(\d+)/);

use IO::Socket ();
@ISA=qw(IO::Socket::INET);

$PROTO = "HTTP/1.1";

=item $d = new HTTP::Daemon

The constructor takes the same parameters as the
I<IO::Socket::INET> constructor.  It can also be called without specifying
any parameters. The daemon will then set up a listen queue of 5
connections and allocate some random port number.  A server that wants
to bind to some specific address on the standard HTTP port will be
constructed like this:

  $d = new HTTP::Daemon
        LocalAddr => 'www.someplace.com',
        LocalPort => 80;

=cut

sub new
{
    my($class, %args) = @_;
    $args{Listen} ||= 5;
    $args{Proto}  ||= 'tcp';
    my $self = $class->SUPER::new(%args);
    return undef unless $self;

    my $host = $args{LocalAddr};
    unless ($host) {
	require Sys::Hostname;
	$host = lc Sys::Hostname::hostname();
    }
    ${*$self}{'httpd_server_name'} = $host;
    $self;
}


=item $c = $d->accept([$pkg])

This method is the same as I<IO::Socket::accept> but returns an
I<HTTP::Daemon::ClientConn> reference by default.  It returns
undef if you specify a timeout and no connection is made within
that time.

=cut

sub accept
{
    my $self = shift;
    my $pkg = shift || "HTTP::Daemon::ClientConn";
    my $sock = $self->SUPER::accept($pkg);
    ${*$sock}{'httpd_daemon'} = $self if $sock;
    $sock;
}


=item $d->url

Returns a URL string that can be used to access the server root.

=cut

sub url
{
    my $self = shift;
    my $url = "http://";
    $url .= ${*$self}{'httpd_server_name'};
    my $port = $self->sockport;
    $url .= ":$port" if $port != 80;
    $url .= "/";
    $url;
}


=item $d->product_tokens

Returns the name that this server will use to identify itself.  This
is the string that is sent with the I<Server> response header.  The
main reason to have this method is that subclasses can override it if
they want to use another product name.

=cut

sub product_tokens
{
    "libwww-perl-daemon/$HTTP::Daemon::VERSION";
}


package HTTP::Daemon::ClientConn;

use vars qw(@ISA $DEBUG);
use IO::Socket ();
@ISA=qw(IO::Socket::INET);
*DEBUG = \$HTTP::Daemon::DEBUG;

use HTTP::Request  ();
use HTTP::Response ();
use HTTP::Status;
use HTTP::Date qw(time2str);
use LWP::MediaTypes qw(guess_media_type);
use Carp ();

my $CRLF = "\015\012";   # "\r\n" is not portable
my $HTTP_1_0 = _http_version("HTTP/1.0");
my $HTTP_1_1 = _http_version("HTTP/1.1");

=back

The I<HTTP::Daemon::ClientConn> is also a I<IO::Socket::INET>
subclass. Instances of this class are returned by the accept() method
of I<HTTP::Daemon>.  The following additional methods are
provided:

=over 4

=item $c->get_request([$headers_only])

Read data from the client and turn it into an
I<HTTP::Request> object which is then returned.  It returns C<undef>
if reading of the request fails.  If it fails, then the
I<HTTP::Daemon::ClientConn> object ($c) should be discarded, and you
should not call this method again.  The $c->reason method might give
you some information about why $c->get_request returned C<undef>.

The $c->get_request method supports HTTP/1.1 request content bodies,
including I<chunked> transfer encoding with footer and self delimiting
I<multipart/*> content types.

The $c->get_request method will normally not return until the whole
request has been received from the client.  This might not be what you
want if the request is an upload of a multi-mega-byte file (and with
chunked transfer encoding HTTP can even support infinite request
messages - uploading live audio for instance).  If you pass a TRUE
value as the $headers_only argument, then $c->get_request will return
immediately after parsing the request headers and you are responsible
for reading the rest of the request content.  If you are going to
call $c->get_request again on the same connection you better read the
correct number of bytes.

=cut

sub get_request
{
    my($self, $only_headers) = @_;
    if (${*$self}{'httpd_nomore'}) {
        $self->reason("No more requests from this connection");
	return;
    }

    $self->reason("");
    my $buf = ${*$self}{'httpd_rbuf'};
    $buf = "" unless defined $buf;
    
    my $timeout = $ {*$self}{'io_socket_timeout'};
    my $fdset = "";
    vec($fdset, $self->fileno, 1) = 1;
    local($_);

  READ_HEADER:
    while (1) {
	# loop until we have the whole header in $buf
	$buf =~ s/^(?:\015?\012)+//;  # ignore leading blank lines
	if ($buf =~ /\012/) {  # potential, has at least one line
	    if ($buf =~ /^\w+[^\012]+HTTP\/\d+\.\d+\015?\012/) {
		if ($buf =~ /\015?\012\015?\012/) {
		    last READ_HEADER;  # we have it
		} elsif (length($buf) > 16*1024) {
		    $self->send_error(413); # REQUEST_ENTITY_TOO_LARGE
		    $self->reason("Very long header");
		    return;
		}
	    } else {
		last READ_HEADER;  # HTTP/0.9 client
	    }
	} elsif (length($buf) > 16*1024) {
	    $self->send_error(414); # REQUEST_URI_TOO_LARGE
	    $self->reason("Very long first line");
	    return;
	}
	print STDERR "Need more data for complete header\n" if $DEBUG;
	return unless $self->_need_more($buf, $timeout, $fdset);
    }
    if ($buf !~ s/^(\w+)[ \t]+(\S+)(?:[ \t]+(HTTP\/\d+\.\d+))?[^\012]*\012//) {
	$self->send_error(400);  # BAD_REQUEST
	$self->reason("Bad request line");
	return;
    }
    my $proto = $3 || "HTTP/0.9";
    my $r = HTTP::Request->new($1, $HTTP::URI_CLASS->new($2, $self->daemon->url));
    $r->protocol($proto);
    ${*$self}{'httpd_client_proto'} = $proto = _http_version($proto);

    if ($proto >= $HTTP_1_0) {
	# we expect to find some headers
	my($key, $val);
      HEADER:
	while ($buf =~ s/^([^\012]*)\012//) {
	    $_ = $1;
	    s/\015$//;
	    if (/^([\w\-]+)\s*:\s*(.*)/) {
		$r->push_header($key, $val) if $key;
		($key, $val) = ($1, $2);
	    } elsif (/^\s+(.*)/) {
		$val .= " $1";
	    } else {
		last HEADER;
	    }
	}
	$r->push_header($key, $val) if $key;
    }

    my $conn = $r->header('Connection');
    if ($proto >= $HTTP_1_1) {
	${*$self}{'httpd_nomore'}++ if $conn && lc($conn) =~ /\bclose\b/;
    } else {
	${*$self}{'httpd_nomore'}++ unless $conn &&
                                           lc($conn) =~ /\bkeep-alive\b/;
    }

    if ($only_headers) {
	${*$self}{'httpd_rbuf'} = $buf;
        return $r;
    }

    # Find out how much content to read
    my $te  = $r->header('Transfer-Encoding');
    my $ct  = $r->header('Content-Type');
    my $len = $r->header('Content-Length');

    if ($te && lc($te) eq 'chunked') {
	# Handle chunked transfer encoding
	my $body = "";
      CHUNK:
	while (1) {
	    print STDERR "Chunked\n" if $DEBUG;
	    if ($buf =~ s/^([^\012]*)\012//) {
		my $chunk_head = $1;
		unless ($chunk_head =~ /^([0-9A-Fa-f]+)/) {
		    $self->send_error(400);
		    $self->reason("Bad chunk header $chunk_head");
		    return;
		}
		my $size = hex($1);
		last CHUNK if $size == 0;

		my $missing = $size - length($buf) + 2; # 2=CRLF at chunk end
		# must read until we have a complete chunk
		while ($missing > 0) {
		    print STDERR "Need $missing more bytes\n" if $DEBUG;
		    my $n = $self->_need_more($buf, $timeout, $fdset);
		    return unless $n;
		    $missing -= $n;
		}
		$body .= substr($buf, 0, $size);
		substr($buf, 0, $size+2) = '';

	    } else {
		# need more data in order to have a complete chunk header
		return unless $self->_need_more($buf, $timeout, $fdset);
	    }
	}
	$r->content($body);

	# pretend it was a normal entity body
	$r->remove_header('Transfer-Encoding');
	$r->header('Content-Length', length($body));

	my($key, $val);
      FOOTER:
	while (1) {
	    if ($buf !~ /\012/) {
		# need at least one line to look at
		return unless $self->_need_more($buf, $timeout, $fdset);
	    } else {
		$buf =~ s/^([^\012]*)\012//;
		$_ = $1;
		s/\015$//;
		if (/^([\w\-]+)\s*:\s*(.*)/) {
		    $r->push_header($key, $val) if $key;
		    ($key, $val) = ($1, $2);
		} elsif (/^\s+(.*)/) {
		    $val .= " $1";
		} elsif (!length) {
		    last FOOTER;
		} else {
		    $self->reason("Bad footer syntax");
		    return;
		}
	    }
	}
	$r->push_header($key, $val) if $key;

    } elsif ($te) {
	$self->send_error(501); 	# Unknown transfer encoding
	$self->reason("Unknown transfer encoding '$te'");
	return;

    } elsif ($ct && lc($ct) =~ m/^multipart\/\w+\s*;.*boundary\s*=\s*(\w+)/) {
	# Handle multipart content type
	my $boundary = "$CRLF--$1--$CRLF";
	my $index;
	while (1) {
	    $index = index($buf, $boundary);
	    last if $index >= 0;
	    # end marker not yet found
	    return unless $self->_need_more($buf, $timeout, $fdset);
	}
	$index += length($boundary);
	$r->content(substr($buf, 0, $index));
	substr($buf, 0, $index) = '';

    } elsif ($len) {
	# Plain body specified by "Content-Length"
	my $missing = $len - length($buf);
	while ($missing > 0) {
	    print "Need $missing more bytes of content\n" if $DEBUG;
	    my $n = $self->_need_more($buf, $timeout, $fdset);
	    return unless $n;
	    $missing -= $n;
	}
	if (length($buf) > $len) {
	    $r->content(substr($buf,0,$len));
	    substr($buf, 0, $len) = '';
	} else {
	    $r->content($buf);
	    $buf='';
	}
    }
    ${*$self}{'httpd_rbuf'} = $buf;

    $r;
}

sub _need_more
{
    my $self = shift;
    #my($buf,$timeout,$fdset) = @_;
    if ($_[1]) {
	my($timeout, $fdset) = @_[1,2];
	print STDERR "select(,,,$timeout)\n" if $DEBUG;
	my $n = select($fdset,undef,undef,$timeout);
	unless ($n) {
	    $self->reason(defined($n) ? "Timeout" : "select: $!");
	    return;
	}
    }
    print STDERR "sysread()\n" if $DEBUG;
    my $n = sysread($self, $_[0], 2048, length($_[0]));
    $self->reason(defined($n) ? "Client closed" : "sysread: $!") unless $n;
    $n;
}

=item $c->read_buffer([$new_value])

Bytes read by $c->get_request, but not used are placed in the I<read
buffer>.  The next time $c->get_request is called it will consume the
bytes in this buffer before reading more data from the network
connection itself.  The read buffer is invalid after $c->get_request
has returned an undefined value.

If you handle the reading of the request content yourself you need to
empty this buffer before you read more and you need to place
unconsumed bytes here.  You also need this buffer if you implement
services like I<101 Switching Protocols>.

This method always return the old buffer content and can optionally
replace the buffer content if you pass it an argument.

=cut

sub read_buffer
{
    my $self = shift;
    my $old = ${*$self}{'httpd_rbuf'};
    if (@_) {
	${*$self}{'httpd_rbuf'} = shift;
    }
    $old;
}


=item $c->reason

When $c->get_request returns C<undef> you can obtain a short string
describing why it happened by calling $c->reason.

=cut

sub reason
{
    my $self = shift;
    my $old = ${*$self}{'httpd_reason'};
    if (@_) {
        ${*$self}{'httpd_reason'} = shift;
    }
    $old;
}


=item $c->proto_ge($proto)

Return TRUE if the client announced a protocol with version number
greater or equal to the given argument.  The $proto argument can be a
string like "HTTP/1.1" or just "1.1".

=cut

sub proto_ge
{
    my $self = shift;
    ${*$self}{'httpd_client_proto'} >= _http_version(shift);
}

sub _http_version
{
    local($_) = shift;
    return 0 unless m,^(?:HTTP/)?(\d+)\.(\d+)$,i;
    $1 * 1000 + $2;
}

=item $c->antique_client

Return TRUE if the client speaks the HTTP/0.9 protocol.  No status
code and no headers should be returned to such a client.  This should
be the same as !$c->proto_ge("HTTP/1.0").

=cut

sub antique_client
{
    my $self = shift;
    ${*$self}{'httpd_client_proto'} < $HTTP_1_0;
}


=item $c->force_last_request

Make sure that $c->get_request will not try to read more requests off
this connection.  If you generate a response that is not self
delimiting, then you should signal this fact by calling this method.

This attribute is turned on automatically if the client announces
protocol HTTP/1.0 or worse and does not include a "Connection:
Keep-Alive" header.  It is also turned on automatically when HTTP/1.1
or better clients send the "Connection: close" request header.

=cut

sub force_last_request
{
    my $self = shift;
    ${*$self}{'httpd_nomore'}++;
}


=item $c->send_status_line( [$code, [$mess, [$proto]]] )

Send the status line back to the client.  If $code is omitted 200 is
assumed.  If $mess is omitted, then a message corresponding to $code
is inserted.  If $proto is missing the content of the
$HTTP::Daemon::PROTO variable is used.

=cut

sub send_status_line
{
    my($self, $status, $message, $proto) = @_;
    return if $self->antique_client;
    $status  ||= RC_OK;
    $message ||= status_message($status) || "";
    $proto   ||= $HTTP::Daemon::PROTO || "HTTP/1.1";
    print $self "$proto $status $message$CRLF";
}

=item $c->send_crlf

Send the CRLF sequence to the client.

=cut


sub send_crlf
{
    my $self = shift;
    print $self $CRLF;
}


=item $c->send_basic_header( [$code, [$mess, [$proto]]] )

Send the status line and the "Date:" and "Server:" headers back to
the client.  This header is assumed to be continued and does not end
with an empty CRLF line.

=cut

sub send_basic_header
{
    my $self = shift;
    return if $self->antique_client;
    $self->send_status_line(@_);
    print $self "Date: ", time2str(time), $CRLF;
    my $product = $self->daemon->product_tokens;
    print $self "Server: $product$CRLF" if $product;
}


=item $c->send_response( [$res] )

Write a I<HTTP::Response> object to the
client as a response.  We try hard to make sure that the response is
self delimiting so that the connection can stay persistent for further
request/response exchanges.

The content attribute of the I<HTTP::Response> object can be a normal
string or a subroutine reference.  If it is a subroutine, then
whatever this callback routine returns is written back to the
client as the response content.  The routine will be called until it
return an undefined or empty value.  If the client is HTTP/1.1 aware
then we will use chunked transfer encoding for the response.

=cut

sub send_response
{
    my $self = shift;
    my $res = shift;
    if (!ref $res) {
	$res ||= RC_OK;
	$res = HTTP::Response->new($res, @_);
    }
    my $content = $res->content;
    my $chunked;
    unless ($self->antique_client) {
	my $code = $res->code;
	$self->send_basic_header($code, $res->message, $res->protocol);
	if ($code =~ /^(1\d\d|[23]04)$/) {
	    # make sure content is empty
	    $res->remove_header("Content-Length");
	    $content = "";
	} elsif ($res->request && $res->request->method eq "HEAD") {
	    # probably OK
	} elsif (ref($content) eq "CODE") {
	    if ($self->proto_ge("HTTP/1.1")) {
		$res->push_header("Transfer-Encoding" => "chunked");
		$chunked++;
	    } else {
		$self->force_last_request;
	    }
	} elsif (length($content)) {
	    $res->header("Content-Length" => length($content));
	} else {
	    $self->force_last_request;
	}
	print $self $res->headers_as_string($CRLF);
	print $self $CRLF;  # separates headers and content
    }
    if (ref($content) eq "CODE") {
	while (1) {
	    my $chunk = &$content();
	    last unless defined($chunk) && length($chunk);
	    if ($chunked) {
		printf $self "%x%s%s%s", length($chunk), $CRLF, $chunk, $CRLF;
	    } else {
		print $self $chunk;
	    }
	}
	print $self "0$CRLF$CRLF" if $chunked;  # no trailers either
    } elsif (length $content) {
	print $self $content;
    }
}


=item $c->send_redirect( $loc, [$code, [$entity_body]] )

Send a redirect response back to the client.  The location ($loc) can
be an absolute or relative URL. The $code must be one the redirect
status codes, and defaults to "301 Moved Permanently"

=cut

sub send_redirect
{
    my($self, $loc, $status, $content) = @_;
    $status ||= RC_MOVED_PERMANENTLY;
    Carp::croak("Status '$status' is not redirect") unless is_redirect($status);
    $self->send_basic_header($status);
    my $base = $self->daemon->url;
    $loc = $HTTP::URI_CLASS->new($loc, $base) unless ref($loc);
    $loc = $loc->abs($base);
    print $self "Location: $loc$CRLF";
    if ($content) {
	my $ct = $content =~ /^\s*</ ? "text/html" : "text/plain";
	print $self "Content-Type: $ct$CRLF";
    }
    print $self $CRLF;
    print $self $content if $content;
    $self->force_last_request;  # no use keeping the connection open
}


=item $c->send_error( [$code, [$error_message]] )

Send an error response back to the client.  If the $code is missing a
"Bad Request" error is reported.  The $error_message is a string that
is incorporated in the body of the HTML entity body.

=cut

sub send_error
{
    my($self, $status, $error) = @_;
    $status ||= RC_BAD_REQUEST;
    Carp::croak("Status '$status' is not an error") unless is_error($status);
    my $mess = status_message($status);
    $error  ||= "";
    $mess = <<EOT;
<title>$status $mess</title>
<h1>$status $mess</h1>
$error
EOT
    unless ($self->antique_client) {
        $self->send_basic_header($status);
        print $self "Content-Type: text/html$CRLF";
	print $self "Content-Length: " . length($mess) . $CRLF;
        print $self $CRLF;
    }
    print $self $mess;
    $status;
}


=item $c->send_file_response($filename)

Send back a response with the specified $filename as content.  If the
file is a directory we try to generate an HTML index of it.

=cut

sub send_file_response
{
    my($self, $file) = @_;
    if (-d $file) {
	$self->send_dir($file);
    } elsif (-f _) {
	# plain file
	local(*F);
	sysopen(F, $file, 0) or 
	  return $self->send_error(RC_FORBIDDEN);
	binmode(F);
	my($ct,$ce) = guess_media_type($file);
	my($size,$mtime) = (stat _)[7,9];
	unless ($self->antique_client) {
	    $self->send_basic_header;
	    print $self "Content-Type: $ct$CRLF";
	    print $self "Content-Encoding: $ce$CRLF" if $ce;
	    print $self "Content-Length: $size$CRLF" if $size;
	    print $self "Last-Modified: ", time2str($mtime), "$CRLF" if $mtime;
	    print $self $CRLF;
	}
	$self->send_file(\*F);
	return RC_OK;
    } else {
	$self->send_error(RC_NOT_FOUND);
    }
}


sub send_dir
{
    my($self, $dir) = @_;
    $self->send_error(RC_NOT_FOUND) unless -d $dir;
    $self->send_error(RC_NOT_IMPLEMENTED);
}


=item $c->send_file($fd);

Copy the file to the client.  The file can be a string (which
will be interpreted as a filename) or a reference to an I<IO::Handle>
or glob.

=cut

sub send_file
{
    my($self, $file) = @_;
    my $opened = 0;
    if (!ref($file)) {
	local(*F);
	open(F, $file) || return undef;
	binmode(F);
	$file = \*F;
	$opened++;
    }
    my $cnt = 0;
    my $buf = "";
    my $n;
    while ($n = sysread($file, $buf, 8*1024)) {
	last if !$n;
	$cnt += $n;
	print $self $buf;
    }
    close($file) if $opened;
    $cnt;
}


=item $c->daemon

Return a reference to the corresponding I<HTTP::Daemon> object.

=cut

sub daemon
{
    my $self = shift;
    ${*$self}{'httpd_daemon'};
}

=back

=head1 SEE ALSO

RFC 2068

L<IO::Socket>, L<Apache>

=head1 COPYRIGHT

Copyright 1996-1998, Gisle Aas

This library is free software; you can redistribute it and/or
modify it under the same terms as Perl itself.

=cut

1;
