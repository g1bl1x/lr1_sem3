package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type NodeF struct {
	val  string
	next *NodeF
}
type ForwardList struct {
	head *NodeF
}

type NodeL struct {
	val        string
	next, prev *NodeL
}
type List struct {
	head, tail *NodeL
}

type Database struct {
	Arrays map[string][]string
	Stacks map[string][]string
	Queues map[string][]string
	FLists map[string]*ForwardList
	LLists map[string]*List
}

func flist_push(l *ForwardList, mode, val, target string) bool {
	n := &NodeF{val: val}
	if mode == "head" || l.head == nil { n.next = l.head; l.head = n; return true }
	curr := l.head
	if mode == "tail" {
		for curr.next != nil { curr = curr.next }
		curr.next = n; return true
	}
	if mode == "before" && l.head.val == target { n.next = l.head; l.head = n; return true }
	for curr != nil {
		if mode == "before" && curr.next != nil && curr.next.val == target {
			n.next = curr.next; curr.next = n; return true
		}
		if mode == "after" && curr.val == target {
			n.next = curr.next; curr.next = n; return true
		}
		curr = curr.next
	}
	return false
}

func flist_pop(l *ForwardList, mode, target string) string {
	if l.head == nil { return "Empty" }
	if mode == "head" {
		v := l.head.val; l.head = l.head.next; return v
	}
	if mode == "tail" {
		if l.head.next == nil { v := l.head.val; l.head = nil; return v }
		curr := l.head
		for curr.next.next != nil { curr = curr.next }
		v := curr.next.val; curr.next = nil; return v
	}
	if mode == "value" {
		if l.head.val == target { l.head = l.head.next; return "OK" }
		curr := l.head
		for curr.next != nil {
			if curr.next.val == target { curr.next = curr.next.next; return "OK" }
			curr = curr.next
		}
	}
	if mode == "before" {
		if l.head.val == target { return "FALSE" }
		if l.head.next != nil && l.head.next.val == target {
			l.head = l.head.next; return "OK"
		}
		curr := l.head
		for curr.next != nil && curr.next.next != nil {
			if curr.next.next.val == target {
				curr.next = curr.next.next; return "OK"
			}
			curr = curr.next
		}
	}
	if mode == "after" {
		curr := l.head
		for curr != nil {
			if curr.val == target {
				if curr.next == nil { return "FALSE" }
				curr.next = curr.next.next; return "OK"
			}
			curr = curr.next
		}
	}
	return "NOT_FOUND"
}

func list_push(l *List, mode, val, target string) bool {
	n := &NodeL{val: val}
	if l.head == nil { l.head = n; l.tail = n; return true }
	if mode == "head" { n.next = l.head; l.head.prev = n; l.head = n; return true }
	if mode == "tail" { n.prev = l.tail; l.tail.next = n; l.tail = n; return true }
	curr := l.head
	for curr != nil {
		if curr.val == target {
			if mode == "before" {
				n.next = curr; n.prev = curr.prev
				if curr.prev != nil { curr.prev.next = n } else { l.head = n }
				curr.prev = n
			} else {
				n.prev = curr; n.next = curr.next
				if curr.next != nil { curr.next.prev = n } else { l.tail = n }
				curr.next = n
			}
			return true
		}
		curr = curr.next
	}
	return false
}

func list_pop(l *List, mode, target string) string {
	if l.head == nil { return "Empty" }
	if mode == "head" {
		v := l.head.val; l.head = l.head.next
		if l.head != nil { l.head.prev = nil } else { l.tail = nil }; return v
	}
	if mode == "tail" {
		v := l.tail.val; l.tail = l.tail.prev
		if l.tail != nil { l.tail.next = nil } else { l.head = nil }; return v
	}
	if mode == "value" {
		curr := l.head
		for curr != nil {
			if curr.val == target {
				if curr.prev != nil { curr.prev.next = curr.next } else { l.head = curr.next }
				if curr.next != nil { curr.next.prev = curr.prev } else { l.tail = curr.prev }
				return "OK"
			}
			curr = curr.next
		}
	}
	if mode == "before" {
		curr := l.head
		for curr != nil {
			if curr.val == target {
				temp := curr.prev
				if temp == nil { return "FALSE" }
				if temp.prev != nil { temp.prev.next = curr } else { l.head = curr }
				curr.prev = temp.prev
				return "OK"
			}
			curr = curr.next
		}
	}
	if mode == "after" {
		curr := l.head
		for curr != nil {
			if curr.val == target {
				temp := curr.next
				if temp == nil { return "FALSE" }
				if temp.next != nil { temp.next.prev = curr } else { l.tail = curr }
				curr.next = temp.next
				return "OK"
			}
			curr = curr.next
		}
	}
	return "NOT_FOUND"
}

func executeQuery(db *Database, queryStr string) string {
	t := strings.Fields(queryStr)
	if len(t) == 0 { return "Error" }
	cmd := strings.ToUpper(t[0])

	if cmd == "PRINT" && len(t) > 1 {
		n := t[1]; var tmp []string
		if v, ok := db.Arrays[n]; ok { tmp = v } else if v, ok := db.Stacks[n]; ok { tmp = v } else if v, ok := db.Queues[n]; ok { tmp = v } else if v, ok := db.FLists[n]; ok { c := v.head; for c != nil { tmp = append(tmp, c.val); c = c.next } } else if v, ok := db.LLists[n]; ok { c := v.head; for c != nil { tmp = append(tmp, c.val); c = c.next } } else { return "Not found" }
		return "[" + strings.Join(tmp, ", ") + "]"
	}
	if strings.HasPrefix(cmd, "M") && len(t) > 1 {
		n := t[1]
		if _, ok := db.Arrays[n]; !ok { db.Arrays[n] = []string{} }
		if cmd == "MPUSH" && len(t) == 3 { db.Arrays[n] = append(db.Arrays[n], t[2]); return t[2] }
		if cmd == "MPUSH" && len(t) == 4 {
			idx, _ := strconv.Atoi(t[2])
			if idx >= 0 && idx <= len(db.Arrays[n]) {
				db.Arrays[n] = append(db.Arrays[n][:idx], append([]string{t[3]}, db.Arrays[n][idx:]...)...); return t[3]
			}
		}
		if cmd == "MGET" {
			idx, _ := strconv.Atoi(t[2]); if idx >= 0 && idx < len(db.Arrays[n]) { return db.Arrays[n][idx] }; return "Out of bounds"
		}
		if cmd == "MDEL" {
			idx, _ := strconv.Atoi(t[2])
			if idx >= 0 && idx < len(db.Arrays[n]) { db.Arrays[n] = append(db.Arrays[n][:idx], db.Arrays[n][idx+1:]...); return "OK" }
		}
	}
	if (strings.HasPrefix(cmd, "F") || strings.HasPrefix(cmd, "L")) && len(t) > 1 {
		n := t[1]; mode := t[2]
		if strings.HasPrefix(cmd, "F") { if _, ok := db.FLists[n]; !ok { db.FLists[n] = &ForwardList{} } } else { if _, ok := db.LLists[n]; !ok { db.LLists[n] = &List{} } }
		if strings.HasSuffix(cmd, "PUSH") {
			val := t[len(t)-1]; target := ""; if len(t) == 5 { target = t[3] }
			var ok bool
			if strings.HasPrefix(cmd, "F") { ok = flist_push(db.FLists[n], mode, val, target) } else { ok = list_push(db.LLists[n], mode, val, target) }
			if ok { return "OK" }; return "FALSE"
		}
		if strings.HasSuffix(cmd, "DEL") {
			tgt := ""; if len(t) == 4 { tgt = t[3] }
			if strings.HasPrefix(cmd, "F") { return flist_pop(db.FLists[n], mode, tgt) } else { return list_pop(db.LLists[n], mode, tgt) }
		}
		if strings.HasSuffix(cmd, "GET") {
			if strings.HasPrefix(cmd, "F") { c := db.FLists[n].head; for c != nil { if c.val == t[2] { return "TRUE" }; c = c.next } } else { c := db.LLists[n].head; for c != nil { if c.val == t[2] { return "TRUE" }; c = c.next } }
			return "FALSE"
		}
	}
	if (strings.HasPrefix(cmd, "S") || strings.HasPrefix(cmd, "Q")) && len(t) > 1 {
		n := t[1]; var c []string; if cmd == "SPUSH" || cmd == "SPOP" { c = db.Stacks[n] } else { c = db.Queues[n] }
		if strings.HasSuffix(cmd, "PUSH") {
			c = append(c, t[2]); if cmd == "SPUSH" { db.Stacks[n] = c } else { db.Queues[n] = c }; return t[2]
		}
		if strings.HasSuffix(cmd, "POP") {
			if len(c) == 0 { return "Empty" }; var v string
			if cmd == "SPOP" { v = c[len(c)-1]; c = c[:len(c)-1]; db.Stacks[n] = c } else { v = c[0]; c = c[1:]; db.Queues[n] = c }
			return v
		}
	}
	return "Unknown command"
}

func fileIO(db *Database, filename string, save bool) {
	if !save {
		data, err := os.ReadFile(filename); if err != nil { return }
		for _, line := range strings.Split(string(data), "\n") {
			t := strings.Fields(line); if len(t) < 3 { continue }
			typeStr, n := t[0], t[1]
			if typeStr == "F" { db.FLists[n] = &ForwardList{} }
			if typeStr == "L" { db.LLists[n] = &List{} }
			for _, val := range t[2:] {
				switch typeStr {
				case "M": db.Arrays[n] = append(db.Arrays[n], val)
				case "F": flist_push(db.FLists[n], "tail", val, "")
				case "L": list_push(db.LLists[n], "tail", val, "")
				case "S": db.Stacks[n] = append(db.Stacks[n], val)
				case "Q": db.Queues[n] = append(db.Queues[n], val)
				}
			}
		}
	} else {
		var sb strings.Builder
		for k, v := range db.Arrays { if len(v) > 0 { sb.WriteString("M " + k + " " + strings.Join(v, " ") + "\n") } }
		for k, v := range db.Stacks { if len(v) > 0 { sb.WriteString("S " + k + " " + strings.Join(v, " ") + "\n") } }
		for k, v := range db.Queues { if len(v) > 0 { sb.WriteString("Q " + k + " " + strings.Join(v, " ") + "\n") } }
		for k, v := range db.FLists { if v.head == nil { continue }; sb.WriteString("F " + k); c := v.head; for c != nil { sb.WriteString(" " + c.val); c = c.next }; sb.WriteString("\n") }
		for k, v := range db.LLists { if v.head == nil { continue }; sb.WriteString("L " + k); c := v.head; for c != nil { sb.WriteString(" " + c.val); c = c.next }; sb.WriteString("\n") }
		_ = os.WriteFile(filename, []byte(sb.String()), 0644)
	}
}

func main() {
	fileFlag := flag.String("file", "database.data", "")
	queryFlag := flag.String("query", "", "")
	flag.Parse(); if *queryFlag == "" { os.Exit(1) }
	db := &Database{
		Arrays: make(map[string][]string), Stacks: make(map[string][]string), Queues: make(map[string][]string),
		FLists: make(map[string]*ForwardList), LLists: make(map[string]*List),
	}
	fileIO(db, *fileFlag, false)
	fmt.Println(executeQuery(db, *queryFlag))
	fileIO(db, *fileFlag, true)
}
