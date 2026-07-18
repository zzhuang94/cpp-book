package main

import "fmt"

type Person struct {
	Name  string
	Age   int
	Hobby []string
}

func main() {
	fmt.Println("Hello, World!")
	p := new(Person)
	fmt.Printf("p: %+v\n", p)
	fmt.Printf("p address: %p\n", p)
	fmt.Printf("p.Name: %s\n", p.Name)
	fmt.Printf("p.Age: %d\n", p.Age)
	fmt.Printf("p.Hobby: %v\n", p.Hobby)
	fmt.Println(p.Hobby == nil)
	p.Hobby = append(p.Hobby, "reading")
	fmt.Printf("p.Hobby: %v\n", p.Hobby)
	fmt.Println(p.Hobby == nil)

	fmt.Println("--------------------------------")

	ss := []string{}
	fmt.Println(ss == nil)
	ss = append(ss, "reading")
	fmt.Println(ss == nil)
	fmt.Printf("ss: %v\n", ss)

	fmt.Println("--------------------------------")

	var nn []string
	fmt.Println(nn == nil)
	nn = append(nn, "reading")
	fmt.Println(nn == nil)
	fmt.Printf("nn: %v\n", nn)
}
