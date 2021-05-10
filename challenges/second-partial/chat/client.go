// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

//!+
func main() {
	user_name := flag.String("user","missing","username")
	server := flag.String("server","missing","irc-server")
	flag.Parse()
	
	if *user_name == "missing" {
		log.Fatalln("Missing argument: user")
	}
	
	if *server == "missing" {
		log.Fatalln("Missing argument: server")
	}

	conn, err := net.Dial("tcp", *server)
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	fmt.Fprintf(conn, *user_name+"\n")
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
 