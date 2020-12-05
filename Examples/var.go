package main

func test() (int,int){
    
}

func main(){
    a:= 50 / (20 * 30 + 40) + 10
    b := 10.10
    _ = 10 + a
    
    
    a,b = test()

    a,b = 10,20
    b,_ = a, 20+30
    a = test()

}

