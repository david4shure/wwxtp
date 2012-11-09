#!/usr/bin/perl
use warnings;
use strict;
use Socket;
use autodie;
use v5.14;
use XML::Simple;

sub error_response {
    my ($client_con, $mess) = @_;
    send($client_con, $mess, 0);
    die($mess);
}

my $port = 4114;
my $protocol = getprotobyname('tcp');
socket(my $sock, AF_INET, SOCK_STREAM, $protocol);

$SIG{__DIE__} = sub {
    close($sock);
    die($_[0])
};

my $listen_addr = INADDR_ANY;

say "bind...";
bind($sock, sockaddr_in($port, INADDR_ANY));

say "listen...";
listen($sock, SOMAXCONN);

say "waiting for a client then accept...";
while (accept(my $client_con, $sock)) {

    say "recv()ing ...";
    recv($client_con, my $buffer, 10000000, 0);
    say "Client Message: $buffer ";

    my $request = XMLin($buffer);
    use Data::Dumper;
    say Dumper($request);

    my $command = $request->{request}{command};
    defined $command or error_response($client_con, "No <command> tag was found");
    $command eq 'RETRIEVE' or error_reponse($client_con, "This server only supports the 'RETRIEVE' command.");
    my $position = $request->{request}{position};
    defined $position or error_response($client_con, "No <position> tag was found");
    my $lat = $position->{lat};
    say $lat;
    my $lon = $position->{lon};
    say $lon;

    sub query_api {
        my ($lat, $lon) = @_;
        my $query_string = "GET /api/e5df4405be67dec2/conditions/q/$lat,$lon.xml\r\n\r\n";
        my $wunderground = "38.102.136.138";
        socket(my $wundersock, AF_INET, SOCK_STREAM, $protocol);
        connect($wundersock, sockaddr_in(80, inet_aton($wunderground)));
        send($wundersock, $query_string, 0);
        my $r = '';
        while (1) {
            recv($wundersock, my $buffer, 10000000, 0);
            return $r if $buffer eq '';
            print $buffer;
            $r .= $buffer;
        }
    }

    my $response = query_api($lat, $lon);
    send($client_con, $response, 0);
    close($client_con);
}
