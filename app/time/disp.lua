cur_time = os.date("%H:%M:%S")
if(label == nil) then
    label = lvgl.Label(nil, {
        x = 0, y = 1,
        text = string.format("%s", cur_time),
        text_color = "#133",
    })
end
label:set({ text = cur_time })
-- print("----boy next door----")