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
            "include_dirs":[
                "<!(node -e \"require('nan')\")"
            ],
            "ldflags":[
                "-lrt"
            ],
            "cflags":[
                "-Wall -O3 -g -Wno-cast-function-type"
            ]
        }
    ]
}