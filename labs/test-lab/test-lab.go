package main

import (
	"fmt"
	"os"
)

func main() {
  if(len(os.Args) > 1) {
    name :=""
    for i := 0; i < len(os.Args)-1; i++ {
      name += " "+os.Args[i+1]
    }
    fmt.Println("Hello"+name+", Welcome to the jungle")
  }else{
    fmt.Println("Error: No arguments passed!")
  }
}
