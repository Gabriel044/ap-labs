// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"net"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type user struct{
	ip_address string
	cli chan string
	connection_date time.Time
	connection net.Conn
}

type cli_user struct{
	username string
	cli chan string
}

var (
	entering = make(chan cli_user) 
	leaving  = make(chan client)
	messages = make(chan string) // all incoming client messages
	connected_users = make(map[string]user)
	loggin_sequence = make([]string,0) // for admin role assignation
	admin string // for current admin storage
)

func broadcaster() {
	clients := make(map[client]string) // all connected clients' channels
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli, username := range clients {
				//If the channel doesn't belong to the sender...
				if(username != strings.Split(msg," ")[0]){
					cli <- "\n"+msg
				}
			}

		case cli_mapping := <-entering:
			clients[cli_mapping.cli] = cli_mapping.username

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}
//!-broadcaster


//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	var in_messages int //number of messages to be recieved before printing "user > "
	var who string
	first:= true
	input := bufio.NewScanner(conn)
	local := time.Now().UTC()
	location,err := time.LoadLocation("America/Mexico_City")
	if err == nil{
		local = local.In(location)
	}
	for input.Scan() {
		if(first){
			//User just logged in
			who = input.Text()
			if len(connected_users) == 0 {
				in_messages = 4 
			}else{
				in_messages = 2
			}
			go clientWriter(conn, ch, who, &in_messages)
			ch <- "irc-server > Welcome to the Simple IRC Server"
			ch <- "irc-server > Your user ["+ who+"] is successfully logged" 
			messages <- "irc-server > New connected user ["+who+"]" 
			fmt.Println("irc-server > New connected user ["+who+"]")
			if(len(connected_users) == 0){
				ch <- "irc-server > Congrats, you were the first user."
				ch <- "irc-server > You're the new IRC Server ADMIN"
				fmt.Println("irc-server > ["+who+"] was promoted as the channel ADMIN")
				admin = who
			}
			entering <- cli_user{who,ch}
			connected_users[who] = user{conn.RemoteAddr().String(),ch,local,conn}
			loggin_sequence = append(loggin_sequence,who)
			first = false
		}else{
			//User is able to send messages now
			incoming_message := input.Text()
			switch {
			case incoming_message == "/users":
				in_messages = len(connected_users)
				getOnlineUsers(ch)
			case incoming_message == "/time":
				getTime(ch, local)
			case strings.HasPrefix(incoming_message,"/user"):
				request := strings.Split(incoming_message," ")
				if len(request) < 2 {
					ch <- "irc-server > It seems like arguments were missing for this command."
				}else{
					request_username := request[1]
					getUserInfo(ch,request_username)
				}
						
			case strings.HasPrefix(incoming_message,"/msg"):
				request := strings.Split(incoming_message," ")
				if len(request) < 3 {
					ch <- "irc-server > It seems like arguments were missing for this command."
				}else{
					destination_user := request[1]
					private_message := request[2:]
					sendPrivateMessage(ch,who,destination_user,strings.Join(private_message," "))
					fmt.Fprint(conn, who+" > ")
				}
				
			case strings.HasPrefix(incoming_message,"/kick"):
				if(admin == who){
					request := strings.Split(incoming_message," ")
					if len(request) < 2 {
						ch <- "irc-server > It seems like arguments were missing for this command."
					}else{
						fmt.Fprint(conn,who+" > ")
						kicked_user := request[1]
						kickUser(ch,kicked_user)
					}		
				}else{
					ch <- "irc-server > Sorry, only the admin is allowed to kick users."
				}

			default:
				messages <- who + " > " + input.Text()
				fmt.Fprint(conn, who+" > ")
			}
		}
		//fmt.Fprint(conn, who+" > ")
	}
	// NOTE: ignoring potential errors from input.Err()
	leaving <- ch
	delete(connected_users,who)
	messages <- "irc-server > ["+who+"] "+"left channel"
	fmt.Println("irc-server > ["+who+"] left")
	if(admin == who){
		//Assign new admin
		loggin_sequence = loggin_sequence[1:]
		//Verifies there are users online to make the oldest admin
		 if len(loggin_sequence) > 0 {
			first_user := loggin_sequence[0]
			fmt.Println("irc-server > ["+first_user+"] was promoted as the channel ADMIN")
			connected_users[first_user].cli <- "\nirc-server > You're the new IRC Server ADMIN"
		}
	}else{
		//delete index of not admin user from loggin_sequence
		removeFromSequence(who)
	}
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string, user string, in_messages *int) {
	num_msg := 0
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
		num_msg ++
		if num_msg == *in_messages {
			fmt.Fprint(conn, user+" > ")
			num_msg = 0
			*in_messages = 1
		}
	}
}
//!-handleConn

func removeFromSequence(who string){
	var deleted_element int
	for index, logged_user := range loggin_sequence {
		if logged_user == who {
			deleted_element = index
			break
		}
	}
	second_half := loggin_sequence[deleted_element+1:]
	loggin_sequence = loggin_sequence[:deleted_element]
	loggin_sequence = append(loggin_sequence,second_half...)
}

//!-Commands
func kickUser(ch chan string, kicked_user string) {
	if user, found := connected_users[kicked_user]; found{
		user.cli <- "irc-server > You're kicked from this channel"
		user.cli <- "irc-server > Bad language is not allowed on this channel"
		user.connection.Close()
		messages <- "irc-server > "+"["+kicked_user+"] was kicked from channel for bad language policy violation"
		fmt.Println("irc-server > ["+kicked_user+"] was kicked")
	}else{
		ch <- "\nirc-server > Sorry, the user you requested for is not currently online."
	}
}

func getUserInfo(ch chan string, request_username string) {
	var message string
	if requested_user,found := connected_users[request_username]; found{
		message = "irc-server > username: "+request_username+", IP: "+requested_user.ip_address+", Connected since: "+requested_user.connection_date.Format("01-02-2006 15:04:05")
	}else{
		message = "irc-server > Sorry, the user you requested for is not currently online."
	}
	ch <- message		
}

func getOnlineUsers(ch chan string) {
	for user_name, user := range connected_users{
		ch <- "irc-server > "+user_name+" - connected since "+user.connection_date.Format("01-02-2006 15:04:05")
	}
}

func sendPrivateMessage(ch chan string, sender_user string, destination_user string, message string){
	if requested_user,found := connected_users[destination_user]; found{
		requested_user.cli <- "\n"+sender_user+" > "+message
	}else{
		ch <- "\nirc-server > Sorry, the user you requested for is not currently online."
	}
}

func getTime(ch chan string, local time.Time) {
	ch <- "irc-server > Local Time: "+local.Location().String()+" "+local.Format("15:04")
}
//!-Commands

//!+main
func main() {
	host := flag.String("host","missing","a server")
	port := flag.String("port","missing","a port"); 
	flag.Parse()

	if *host == "missing"{
		log.Fatalln("Missing argument: host")
	}

	if *port == "missing" {
		log.Fatalln("Missing argument: port")
	}

	listener, err := net.Listen("tcp", *host+":"+*port)

	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("irc-server > Simple IRC Server started at "+*host+":"+*port)
	go broadcaster()
	fmt.Println("irc-server > Ready for receiving new clients")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
