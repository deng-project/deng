package main
import "fmt"

func main() {
    for i := 1; i <= 12; i++ {
        fmt.Printf("case NEKO_KEY_F%d:\n\treturn ImGuiKey_F%d;\n\n", i, i)
    }
}
