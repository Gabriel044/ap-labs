// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"fmt"
	"math"
	"math/rand"

	"os"
	"strconv"
)

type Point struct{ x, y float64 }

func (p Point) X() float64{
	return p.x
}

func (p Point) Y() float64{
	return p.y
}
// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}



//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	fmt.Printf("  - ")
	for i := range path {
		if i > 0 {
			if i > 1 {
				fmt.Printf("+ ")
			}
			sum += path[i-1].Distance(path[i])
			fmt.Printf("%f ",path[i-1].Distance(path[i]))
		}
	}
	sum += path[len(path)-1].Distance(path[0])
	fmt.Printf("+ %f ",path[len(path)-1].Distance(path[0]))
	fmt.Printf("= %f\n",sum)
	return sum
}

func main() {
	points , error := strconv.Atoi(os.Args[1])
	if(error == nil){
		path := make(Path, points)
		fmt.Printf("- Generating a [%d] sides figure\n",points)
		fmt.Printf("- Figure's vertices\n")
		for i := 0; i < points; i++ {
			point := Point {(rand.Float64()+1)*10, (rand.Float64()+1)*10}
			fmt.Printf("  - (%f, %f)\n",point.X(),point.Y())
			path[i] = point
		}
		fmt.Printf("- Figure's Perimeter\n")
		path.Distance()
	}else{
		fmt.Printf("Not enough arguments or tried to use something else than an int\n")
	}
	
}

//!-path
