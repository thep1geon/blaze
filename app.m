class Point {
    @new(x, y) {
        self.x = x;
        self.y = y;
    }
    
    print() {
        print("x: {this.x}, y: {this.y}");
    }
}

func add(a, b) {
    return a + b;
}

func main() {
    vasm(
        "call Add 32 64 | x", 
        "print x"
    );

    let x = 32;
    let y = 64;
    let z = (x+y)*x;

    let w = [21, 32, 43];

    print(w[2]);
    print(w);

    for item : w {
        print(item);
    }

    for i : 0..w.len {
        print(w[i]);
    }

    print(x);
    print(y);
    print(z);

    if x == 32 {
        print(add(42, 27));
    } else {
        print(add(42, -27));
    }

    p = Point::new(3, 4);
    p.print();

    z += 54;

    print(++z);
    print((0..w.len)[0]);
}
