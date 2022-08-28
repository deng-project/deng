package main
import "fmt"

func main() {
    for i := 0; i <= 'Z' - 'A'; i++ {
        fmt.Printf("case NEKO_KEY_%c:\n\treturn ImGuiKey_%c;\n\n", rune(i + 'A'), rune(i + 'A'))
    }
}
