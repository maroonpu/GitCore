#!/usr/bin/expect
set ip [lindex $argv 0]
if {$argc<1} {
	puts "Wrong Format! e.g. 192.168.0.x -> ./sendFile x"
}
spawn scp aux root@192.168.0.$ip:/root
expect "password:"
send "12345678\n"
interact