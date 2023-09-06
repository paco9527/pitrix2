cur_time = os.time()
-- recv = '{"results":[{"location":{"id":"WTMKQ069CCJ7","name":"杭州","country":"CN","path":"杭州,杭州,浙江,中国","timezone":"Asia/Shanghai","timezone_offset":"+08:00"},"now":{"text":"多云","code":"4","temperature":"29"},"last_update":"2023-07-01T09:34:31+08:00"}]}'

-- socket存在则立即执行
if(socket ~= nil) then
    if(cur_time - last_time >= 1800) then
        net.timed_write(socket, get_msg) -- 判断是否发送失败
        last_time = cur_time

        if(recv == nil) then
            recv = net.timed_read(socket, 1024)
        end
        
        -- 如果接收到内容则立即执行
        if(recv ~= nil) then
            result = cjson.decode(recv)
            city = result["results"][1]["now"]
            temperature = city["temperature"] -- 气温
            weather_text = city["text"] -- 天气现象代码参考 https://seniverse.yuque.com/hyper_data/api_v3/yev2c3
            print(weather_text)
            label:set({ text = tostring(temperature) })
            icon:set({
                src = icon_table[weather_text]
            })
            recv = nil
        end
    end
end
