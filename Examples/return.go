package main




func main() {
    c:=10
    c = c / 0
    test2()
}

func test2() (int){
    b:=10
    if b < 4{
        a := 10
        for a:=1; a < 10; a = a+1{
            if a < 5{
                return 5
            }else{
                return 5
            }   
        } 
    }else{
        return 5
    }
}


