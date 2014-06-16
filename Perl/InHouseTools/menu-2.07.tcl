########################################################################
######## <menu> ########
########################################################################
	menu .menu -tearoff 0
################
	.menu add cascade -menu .menu.file -label "File" -underline 0
########
	menu .menu.file -tearoff 1
########
	.menu.file add command \
		-accelerator "Ctrl+x" \
		-command {
			destroy .

			set CHANNEL_ID [open ${::env(HOME)}/.j_VARs-$::VERSION.ini w]
			puts -nonewline $CHANNEL_ID "### j_VARs-$::VERSION.ini\n\n\n"
			foreach var $::j_VARs {
				fprintf $CHANNEL_ID 0 $var
			}
			puts -nonewline $CHANNEL_ID "return OK\n"
			close $CHANNEL_ID
		} \
		-label "Exit" \
		-underline 0

	.menu.file add command \
		-accelerator "Ctrl+q" \
		-command {
			destroy .
		} \
		-label "Quit" \
		-underline 0
########
	.menu.file add separator
########
	.menu.file add command \
		-accelerator "Ctrl+o" \
		-command {
			::job::j_LOAD
		} \
		-label "Load Variables" \
		-underline 0

	.menu.file add command \
		-accelerator "Ctrl+s" \
		-command {
			::job::j_SAVE
		} \
		-label "Save Variables" \
		-underline 0
################
	.menu add cascade -menu .menu.run -label "Run" -underline 0
########
	menu .menu.run -tearoff 1
########
	.menu.run add command \
		-accelerator "Ctrl+r" \
		-command {
			::job::run
		} \
		-label "Run" \
		-underline 0
########
	.menu.run add separator
########
	.menu.run add command \
		-accelerator "Ctrl+c" \
		-command {
			uplevel #0 source [file join $ROOTPATH chunk-$::VERSION.tcl]
		} \
		-label "Chunk" \
		-underline 0
################
	.menu add cascade -menu .menu.about -label "About" -underline 0
########
	menu .menu.about -tearoff 1
########
	.menu.about add command \
		-accelerator "<F1>" \
		-command {
			printf "DigiScape Job Template Version $::VERSION"
		} \
		-label "About..." \
		-underline 0
########
	.menu.about add separator
########
	.menu.about add command \
		-accelerator "Ctrl+p" \
		-command {
			xprintf xprintf printHash
		} \
		-label "print all..." \
		-underline 0
################

	. configure -menu .menu
########################################################################
######## </menu> ########
########################################################################

########################################################################
######## <bind> ########
########################################################################
	bind . <F1> {
		printf "DigiScape Job Template Version $::VERSION
	}
	bind . <Control-KeyRelease-p> {
		xprintf xprintf printHash
	}
####	bind . <Alt-KeyPress-q> "destroy ."
	bind . <Control-KeyRelease-q> {
		destroy .
	}
	bind . <Control-KeyRelease-x> {
		destroy .

		set CHANNEL_ID [open ${::env(HOME)}/.j_VARs-$::VERSION.ini w]
		puts -nonewline $CHANNEL_ID "### j_VARs-$::VERSION.ini\n\n\n"
		foreach var $::j_VARs {
			fprintf $CHANNEL_ID 0 $var
		}
		puts -nonewline $CHANNEL_ID "return OK\n"
		close $CHANNEL_ID
	}
	bind . <Control-KeyRelease-r> {
		::job::run
	}
	bind . <Control-KeyRelease-c> {
		uplevel #0 source [file join $ROOTPATH chunk-$::VERSION.tcl]
	}
	bind . <Control-KeyRelease-o> {
		::job::j_LOAD
	}
	bind . <Control-KeyRelease-s> {
		::job::j_SAVE
	}

########################################################################
######## </bind> ########
########################################################################
