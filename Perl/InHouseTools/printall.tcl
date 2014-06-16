proc xxx x {
	upvar $x ref_x
	if [array exists ref_x] {
		puts "$x: is array"
		foreach key [array names ref_x] {
			puts "$key=>$ref_x($key)"
		}
	} else {
		puts "$x: $ref_x"
	}
}
