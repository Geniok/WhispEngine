function Init()
	-- MoveTank.lua

	MoveTank = {}

	MoveTank.Variables = {
		-- Public Variables [All must be initialized]
		speed = 7.1 --[Slider(0, 10)]
	}

	function MoveTank:Start()
	end

	function MoveTank:Update()
		--Movement
		local pos = transform.position

		if input.getKey("w") then
			transform:SetPositionv(pos + (transform.forward * (var.speed * Time.deltaTime)))
		end

		pos = transform.position

		if input.getKey("a") then
			transform:SetPositionv(pos + (transform.right * (var.speed * Time.deltaTime)))
		end

		pos = transform.position

		if input.getKey("s") then
			transform:SetPositionv(pos - (transform.forward * (var.speed * Time.deltaTime)))
		end

		pos = transform.position

		if input.getKey("d") then
			transform:SetPositionv(pos - (transform.right * (var.speed * Time.deltaTime)))
		end

	end

	return MoveTank
end





