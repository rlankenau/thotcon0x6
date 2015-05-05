#!/usr/bin/perl -w
use Device::SerialPort;
my $port = "";
do {
    $port = `ls /dev/* | grep ACM`;
    chomp $port;
} while ($port eq "");
print "Trying $port";

my $serial = Device::SerialPort->new($port);
$serial->baudrate(1200);
$serial->write_settings;
$serial->pulse_dtr_on(100);
$serial->close();

$port="";
do {
    $port = `ls /dev/* | grep ACM`;
    chomp $port;
} while ($port eq "");
system("sudo /usr/share/arduino/hardware/tools/avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -v -patmega32u4 -cavr109 -b57600 -P$port @ARGV");

