type Point {
    x: Str, 
    y: Num
};

func print_point p: Point {
    print "Point: {";
    print p.x;
    print p.y;
    print "}";
}

func read p: Ptr {
    vasm "read {p} | tmp"
}

func add a b {
    return a + b;
}

func main {
    vasm "call Add 32 64 | x", "print x";

    let x: Num = 32;
    let y: Str = 64; # This is basically a pointer but still carries 
                     # number proprties. It's just how it's used by
                     # the compiler
    let p: Ptr = 0;

    let data = read p;

    let z = (x + y) * x;

    let w = [21, 32, 43];

    print w[2];
    print w;

    for item : w {
        print(item);
    }

    for i : 0..len w {
        print w[i];
    }

    print x;
    print y;
    print z;

    if x == 32 {
        print add 42, 27;
    } 
    else {
        print add 42, -27;
    }

    let p: Point = {10, 32};

    print p.x;

    z += 54;

    print ++z;
    print (0..len w)[0];
}
