RawOn programming language. 

A minimal one.

```
fn fibo(x) {
    if (x < 2) {
        return x
    } else {
        return fibo(x - 2) + fibo(x - 1)
    }
}

print('Enter n: ')

n = input() >> toint 

// equivalent to:
// n = toint(input())

print(n, "-th fibonacci number: ", fibo(n), "\n")
```

Of course, a toy and an incomplete one.
