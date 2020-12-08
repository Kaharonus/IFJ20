package main

func test() (int,int){
    a:= 10
    return 1, 2+3
    a = 5
}

func main(){

    c := 20
    
    d := "d"
    
    j := "ffff"
    if j == d {
    }else{
    }
    
    a:= 50 / (20 * 30 + 40) + 10
    a, _ = test()

    b := 10.10
    _ = 10 + a
    
    if a > b{
        a = b
    }
    
    else{
        b = a
    }

    a,b = 10,20
    b,_ = a, 20+30
    a, b = test()

}

