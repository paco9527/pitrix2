-- print("dungeon")
cur_time = os.date("%H:%M:%S")
if(label0 == nil) then
    str = cur_time
    label0 = lvgl.Label(nil, {
        x = 1, y = 1,
        text_color = "#133",
        text = tostring(233),
    })
end
year = 1998
year = tostring(year)
label0:set({ text = tostring(year) })
-- print("ass")