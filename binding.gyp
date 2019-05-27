
{

    "targets": [
        {
            "target_name": "rpi-ws218x",

            "sources": [
            	"src/cpp/addon.cpp"
            ],

            "include_dirs": [
	            "./hzeller/include",
                "<!(node -e \"require('nan')\")"
            ],

            "link_settings": {
	            "libraries": [
					"<!(pwd)/hzeller/lib/librgbmatrix.a"
	            ]
            },

			"ldflags"     : ["-lrt -lm -lpthread"],
			"ldflags!"    : ["-rdynamic", "-shared"],

			"cflags"      : ["-Wall -O3 -g" ],
			"cflags!"     : ["-Wno-unused-parameter", "-Wextra", "-fno-omit-frame-pointer", "-std=gnu++0x", "-fno-exceptions", "-fno-rtti" ],

			"cflags_cc"   : ["-Wall -g -fno-strict-aliasing -O2 -pthread"],
			"cflags_cc!"  : ["-Wno-unused-parameter", "-Wextra", "-fno-omit-frame-pointer", "-fno-exceptions", "-fno-rtti" ],


            "conditions": [["OS=='mac'", {"xcode_settings": {"GCC_ENABLE_CPP_EXCEPTIONS": "YES"}}]]

        }


    ]
}
