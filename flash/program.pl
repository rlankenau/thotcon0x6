#!/usr/bin/perl -w
use Device::SerialPort;
my $try = 0;
do {
    my $port = "";
    do {
        $port = `ls /dev/* | grep ACM`;
        chomp $port;
        print ".";
    } while ($port eq "");
    print "\nTrying $port\n";
    sleep 1;
    my $serial = Device::SerialPort->new($port);
    $serial->databits(8);
    $serial->baudrate(1200);
    $serial->parity("none");
    $serial->stopbits(1);
    $serial->handshake("rts");
    $serial->write_settings;
    $serial->pulse_dtr_on(100);
    $serial->close();

    $port="";
    do {
        $port = `ls /dev/* | grep ACM`;
        chomp $port;
        print ".";
    } while ($port eq "");
    sleep 1;
    if (system("sudo /usr/share/arduino/hardware/tools/avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -v -patmega32u4 -cavr109 -b57600 -P$port @ARGV") == 0)
    {
        print "\nSuccess!\n";
        exit;
    }
    $try++;
} while ($try < 10);
print "\nBurning failed!\n";

