static char test_script[] = {
    "str = \"ass\""
    "draw_string(0,0,str)"
    "write_buf()"
};

static char test_script_2[] = {
    "cur_time = os.date(\"%H:%M:%S\")\nprint(cur_time)\n"
    "print(cur_time)"
    "ass.print_ass()"
};

loader_init();
app_load(test_script);
script_exec(test_script);
app_load_from_file("app/time/", READ_FROM_RAM);
loader_deinit();