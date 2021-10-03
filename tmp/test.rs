extern "Rust" { fn printI(x: i32); }

fn main() {
	unsafe {printI(6);}
}