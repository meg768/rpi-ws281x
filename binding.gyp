{  
    "targets":[  
        {  
            "target_name":"rpi-ws281x",
            "sources":[  
                "src/addon.cpp",
                "./src/rpi_ws281x/ws2811.c",
                "./src/rpi_ws281x/pwm.c",
                "./src/rpi_ws281x/dma.c",
                "./src/rpi_ws281x/mailbox.c",
                "./src/rpi_ws281x/rpihw.c",
                "./src/rpi_ws281x/pcm.c"
            ],
            "xxxsources":[  
                "src/addon.cpp"
            ],
            "xxxlink_settings": {
	            "libraries": [
					"<!(pwd)/src/rpi_ws281x/libws2811.a"
	            ]
            },

            "include_dirs":[  
                "<!(node -e \"require('nan')\")"
            ],
            "ldflags":[  
                "-lrt"
            ],
            "xxxldflags!":[  
                "-rdynamic",
                "-shared"
            ],
            "cflags":[  
                "-Wall -O3 -g"
            ],
            "xxxcflags!":[  
                "-Wno-unused-parameter",
                "-Wextra",
                "-fno-omit-frame-pointer",
                "-std=gnu++0x",
                "-fno-exceptions",
                "-fno-rtti"
            ],
            "xxxcflags_cc":[  
                "-Wall -g -fno-strict-aliasing -O2 -pthread"
            ],
            "xxxcflags_cc!":[  
                "-Wno-unused-parameter",
                "-Wextra",
                "-fno-omit-frame-pointer",
                "-fno-exceptions",
                "-fno-rtti"
            ],
            "conditions":[  
                [  
                    "OS=='mac'",
                    {  
                        "xcode_settings":{  
                            "GCC_ENABLE_CPP_EXCEPTIONS":"YES"
                        }
                    }
                ]
            ]
        }
    ]
}