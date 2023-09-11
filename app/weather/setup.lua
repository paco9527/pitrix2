last_time = 0
retry = 0
ret = 0
-- 在心知天气官网申请API后，替换API的私钥到YOUR_SECRET_KEY
if(get_msg == nil) then
    get_msg = 'GET https://api.seniverse.com/v3/weather/now.json?'..
            'key=YOUR_SECRET_KEY&location=ip&language=en&unit=c\r\n\r\n'
end

if(label == nil) then
    label = lvgl.Label(nil, {
        x = 20, y = 1,
        text = string.format("NA"),
        text_color = "#133",
    })
end

if(icon == nil) then
    icon_table = {}

    -- 天气现象见文档https://seniverse.yuque.com/hyper_data/api_v3/yev2c3
    -- 返回的json中，天气现象文字只有首字母是大写
    icon_table["Sunny"] = "A:" .. APPROOT .. "/icons/sunny.png"
    icon_table["Clear"] = "A:" .. APPROOT .. "/icons/clear.png"
    icon_table["Fair"] = icon_table["Sunny"]
    icon_table["Fair" .. "night"] = icon_table["Clear"]
    icon_table["Cloudy"] = "A:" .. APPROOT .. "/icons/cloudy.png"
    icon_table["Mostly cloudy"] = "A:" .. APPROOT .. "/icons/mostly-cloudy.png"
    icon_table["Mostly cloudy" .. "night"] = "A:" .. APPROOT .. "/icons/mostly-cloudy-night.png"
    icon_table["Partly cloudy"] = icon_table["Mostly cloudy"]
    icon_table["Partly cloudy" .. "night"] = icon_table["Mostly cloudy" .. "night"]
    icon_table["Overcast"] = "A:" .. APPROOT .. "/icons/overcast.png"
    icon_table["Shower"] = "A:" .. APPROOT .. "/icons/shower.png"
    icon_table["Light rain"] = "A:" .. APPROOT .. "/icons/light-rain.png"
    icon_table["Moderate rain"] = "A:" .. APPROOT .. "/icons/moderate-rain.png"
    icon_table["Heavy rain"] = "A:" .. APPROOT .. "/icons/heavy-rain.png"
    icon_table["Storm"] = "A:" .. APPROOT .. "/icons/storm.png"
    icon_table["Heavy storm"] = icon_table["Storm"]
    icon_table["Severe storm"] = icon_table["Storm"]
    icon_table["Ice rain"] = "A:" .. APPROOT .. "/icons/ice-rain.png"
    icon_table["Sleet"] = "A:" .. APPROOT .. "/icons/sleet.png"
    icon_table["Light snow"] = "A:" .. APPROOT .. "/icons/light-snow.png"
    icon_table["Moderate snow"] = "A:" .. APPROOT .. "/icons/moderate-snow.png"
    icon_table["Heavy snow"] = "A:" .. APPROOT .. "/icons/heavy-snow.png"
    icon_table["Snowstorm"] = "A:" .. APPROOT .. "/icons/snowstorm.png"
    icon_table["Foggy"] = "A:" .. APPROOT .. "/icons/foggy.png"
    icon_table["Haze"] = "A:" .. APPROOT .. "/icons/haze.png"
    icon = lvgl.Image(nil, {
        src = icon_table["sunny"],
        x = 2,
    })
end
