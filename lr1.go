package main

import (
	"flag"
	"fmt"
	"os"
	"slices"
	"strconv"
	"strings"
)

// 1. АВЛ-ДЕРЕВО
type NodeT struct {
	val         string
	h           int
	left, right *NodeT
}

func getH(n *NodeT) int { if n == nil { return 0 }; return n.h }
func getB(n *NodeT) int { if n == nil { return 0 }; return getH(n.left) - getH(n.right) }
func updateH(n *NodeT)  { if n != nil { n.h = 1 + max(getH(n.left), getH(n.right)) } }

func rotate(p *NodeT, left bool) *NodeT {
	var q *NodeT
	if left {
		q = p.right; p.right = q.left; q.left = p
	} else {
		q = p.left; p.left = q.right; q.right = p
	}
	updateH(p); updateH(q)
	return q
}

func balance(n *NodeT) *NodeT {
	updateH(n)
	if getB(n) > 1 {
		if getB(n.left) < 0 { n.left = rotate(n.left, true) }
		return rotate(n, false)
	}
	if getB(n) < -1 {
		if getB(n.right) > 0 { n.right = rotate(n.right, false) }
		return rotate(n, true)
	}
	return n
}

func avlInsert(n *NodeT, val string) *NodeT {
	if n == nil { return &NodeT{val: val, h: 1} }
	if val < n.val { n.left = avlInsert(n.left, val) } else if val > n.val { n.right = avlInsert(n.right, val) }
	return balance(n)
}

func avlRemove(n *NodeT, val string) *NodeT {
	if n == nil { return nil }
	if val < n.val { n.left = avlRemove(n.left, val) } else if val > n.val { n.right = avlRemove(n.right, val) } else {
		if n.left == nil || n.right == nil {
			if n.left != nil { return n.left }; return n.right
		}
		minNode := n.right
		for minNode.left != nil { minNode = minNode.left }
		n.val = minNode.val
		n.right = avlRemove(n.right, minNode.val)
	}
	return balance(n)
}

func avlFind(n *NodeT, val string) bool {
	if n == nil { return false }
	if n.val == val { return true }
	if val < n.val { return avlFind(n.left, val) }; return avlFind(n.right, val)
}

func avlCollect(n *NodeT, res *[]string) {
	if n == nil { return }
	avlCollect(n.left, res); *res = append(*res, n.val); avlCollect(n.right, res)
}

// 2. СТРУКТУРА БАЗЫ ДАННЫХ И ХЕЛПЕРЫ
type Database struct {
	Arrays map[string][]string
	FLists map[string][]string
	LLists map[string][]string
	Stacks map[string][]string
	Queues map[string][]string
	Trees  map[string]*NodeT
}

func listPush(l []string, mode, val, target string) ([]string, bool) {
	if mode == "head" { return append([]string{val}, l...), true }
	if mode == "tail" { return append(l, val), true }
	idx := slices.Index(l, target)
	if idx == -1 { return l, false }
	if mode == "before" {
		return append(l[:idx], append([]string{val}, l[idx:]...)...), true
	}
	return append(l[:idx+1], append([]string{val}, l[idx+1:]...)...), true
}

// 3. ОБРАБОТЧИК ЗАПРОСОВ
func executeQuery(db *Database, queryStr string) string {
	t := strings.Fields(queryStr)
	if len(t) == 0 { return "Error" }
	cmd := strings.ToUpper(t[0])

	if cmd == "PRINT" && len(t) > 1 {
		n := t[1]
		var tmp []string
		if v, ok := db.Arrays[n]; ok { tmp = v } else if v, ok := db.Stacks[n]; ok { tmp = v } else if v, ok := db.Queues[n]; ok { tmp = v } else if v, ok := db.FLists[n]; ok { tmp = v } else if v, ok := db.LLists[n]; ok { tmp = v } else if v, ok := db.Trees[n]; ok { avlCollect(v, &tmp) } else { return "Not found" }
		return "[" + strings.Join(tmp, ", ") + "]"
	}

	// Массивы
	if strings.HasPrefix(cmd, "M") && len(t) > 1 {
		n := t[1]
		if cmd == "MPUSH" && len(t) == 3 { db.Arrays[n] = append(db.Arrays[n], t[2]); return t[2] }
		if cmd == "MPUSH" && len(t) == 4 {
			idx, _ := strconv.Atoi(t[2])
			if idx >= 0 && idx <= len(db.Arrays[n]) {
				db.Arrays[n] = append(db.Arrays[n][:idx], append([]string{t[3]}, db.Arrays[n][idx:]...)...)
				return t[3]
			}
		}
		if cmd == "MGET" {
			idx, _ := strconv.Atoi(t[2])
			if idx >= 0 && idx < len(db.Arrays[n]) { return db.Arrays[n][idx] }; return "Out of bounds"
		}
		if cmd == "MDEL" {
			idx, _ := strconv.Atoi(t[2])
			if idx >= 0 && idx < len(db.Arrays[n]) {
				db.Arrays[n] = append(db.Arrays[n][:idx], db.Arrays[n][idx+1:]...); return "OK"
			}
		}
	}
	// Списки
	if (strings.HasPrefix(cmd, "F") || strings.HasPrefix(cmd, "L")) && len(t) > 1 {
		n := t[1]; mode := t[2]
		l := db.FLists[n]; if strings.HasPrefix(cmd, "L") { l = db.LLists[n] }
		
		if strings.HasSuffix(cmd, "PUSH") {
			target := ""; if len(t) == 5 { target = t[3] }
			val := t[len(t)-1]
			res, ok := listPush(l, mode, val, target)
			if strings.HasPrefix(cmd, "F") { db.FLists[n] = res } else { db.LLists[n] = res }
			if ok { return "OK" }; return "FALSE"
		}
		if strings.HasSuffix(cmd, "DEL") {
			if len(l) == 0 { return "Empty" }
			if mode == "head" { v := l[0]; l = l[1:]; if cmd[0] == 'F' { db.FLists[n] = l } else { db.LLists[n] = l }; return v }
			if mode == "tail" { v := l[len(l)-1]; l = l[:len(l)-1]; if cmd[0] == 'F' { db.FLists[n] = l } else { db.LLists[n] = l }; return v }
			if mode == "value" {
				idx := slices.Index(l, t[3])
				if idx != -1 {
					l = append(l[:idx], l[idx+1:]...); if cmd[0] == 'F' { db.FLists[n] = l } else { db.LLists[n] = l }; return "OK"
				}
				return "NOT_FOUND"
			}
		}
		if strings.HasSuffix(cmd, "GET") {
			if slices.Contains(l, t[2]) { return "TRUE" }; return "FALSE"
		}
	}
	// Стек (S) и Очередь (Q)
	if (strings.HasPrefix(cmd, "S") || strings.HasPrefix(cmd, "Q")) && len(t) > 1 {
		n := t[1]
		var c []string
		if cmd[0] == 'S' { c = db.Stacks[n] } else { c = db.Queues[n] }

		if strings.HasSuffix(cmd, "PUSH") {
			c = append(c, t[2]); if cmd[0] == 'S' { db.Stacks[n] = c } else { db.Queues[n] = c }; return t[2]
		}
		if strings.HasSuffix(cmd, "POP") {
			if len(c) == 0 { return "Empty" }
			var v string
			if cmd[0] == 'S' {
				v = c[len(c)-1]; c = c[:len(c)-1]; db.Stacks[n] = c
			} else {
				v = c[0]; c = c[1:]; db.Queues[n] = c
			}
			return v
		}
	}
	// Деревo
	if strings.HasPrefix(cmd, "T") || cmd == "ISMEMBER" {
		n := t[1]; v := t[2]; if cmd == "ISMEMBER" { v = t[2] }
		if cmd == "TINSERT" { db.Trees[n] = avlInsert(db.Trees[n], v); return v }
		if cmd == "TDEL" { db.Trees[n] = avlRemove(db.Trees[n], v); return "OK" }
		if cmd == "TGET" || cmd == "ISMEMBER" {
			if avlFind(db.Trees[n], v) { return "TRUE" }; return "FALSE"
		}
	}
	return "Unknown command"
}

// 4. MAIN И ФАЙЛОВЫЙ ВВОД-ВЫВОД
func fileIO(db *Database, filename string, save bool) {
	if !save {
		data, err := os.ReadFile(filename)
		if err != nil { return }
		for _, line := range strings.Split(string(data), "\n") {
			t := strings.Fields(line)
			if len(t) < 3 { continue }
			typeStr, n := t[0], t[1]
			for _, val := range t[2:] {
				switch typeStr {
				case "M": db.Arrays[n] = append(db.Arrays[n], val)
				case "F": db.FLists[n] = append(db.FLists[n], val)
				case "L": db.LLists[n] = append(db.LLists[n], val)
				case "S": db.Stacks[n] = append(db.Stacks[n], val)
				case "Q": db.Queues[n] = append(db.Queues[n], val)
				case "T": db.Trees[n] = avlInsert(db.Trees[n], val)
				}
			}
		}
	} else {
		var sb strings.Builder
		wr := func(p string, m map[string][]string) {
			for k, v := range m {
				if len(v) > 0 { sb.WriteString(p + " " + k + " " + strings.Join(v, " ") + "\n") }
			}
		}
		wr("M", db.Arrays); wr("F", db.FLists); wr("L", db.LLists); wr("S", db.Stacks); wr("Q", db.Queues)
		for k, v := range db.Trees {
			var tmp []string; avlCollect(v, &tmp)
			if len(tmp) > 0 { sb.WriteString("T " + k + " " + strings.Join(tmp, " ") + "\n") }
		}
		_ = os.WriteFile(filename, []byte(sb.String()), 0644)
	}
}

func main() {
	fileFlag := flag.String("file", "database.data", "")
	queryFlag := flag.String("query", "", "")
	flag.Parse()
	if *queryFlag == "" { os.Exit(1) }

	db := &Database{
		Arrays: make(map[string][]string), FLists: make(map[string][]string), LLists: make(map[string][]string),
		Stacks: make(map[string][]string), Queues: make(map[string][]string), Trees: make(map[string]*NodeT),
	}

	fileIO(db, *fileFlag, false)
	fmt.Println(executeQuery(db, *queryFlag))
	fileIO(db, *fileFlag, true)
}
