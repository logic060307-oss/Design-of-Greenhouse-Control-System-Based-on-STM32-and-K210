<template>
	<view class="wrap">
		<view class="dev-area">
<!-- 			温度卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">温度</view>
					<image class="dev-logo" src="/static/Temp.png" mode=""></image>
				</view>
				<view class="dev-data">{{Temperature}}℃</view>
			</view>
<!-- 			湿度卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">湿度</view>
					<image class="dev-logo" src="/static/humi.png" mode=""></image>
				</view>
				<view class="dev-data">{{Humidity}}%</view>
			</view>
<!-- 			光照强度卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">光照强度</view>
					<image class="dev-logo" src="/static/Light.png" mode=""></image>
				</view>
				<view class="dev-data">{{Light_lux}}lx</view>
			</view>
<!-- 			二氧化碳卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">二氧化碳</view>
					<image class="dev-logo" src="/static/CO2.png" mode=""></image>
				</view>
				<view class="dev-data">{{ccs_data}}<text class="unit">ppm</text></view>
			</view>
<!-- 			土壤湿度卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">土壤湿度</view>
					<image class="dev-logo" src="/static/solihumi.png" mode=""></image>
				</view>
				<view class="dev-data">{{soil_humi}}%</view>
			</view>
<!-- 			动物类型卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">动物入侵</view>
					<image class="dev-logo" src="/static/animal.png" mode=""></image>
				</view>
				<view class="dev-data">{{AnimalType}}</view>
			</view>
<!-- 			led卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">灯光</view>
					<image class="dev-logo" src="/static/led.png" mode=""></image>
				</view>
				<switch :checked="led" @change="onLedSwitch" color="#3b82f6"/>
			</view>
<!-- 			风扇卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">风扇</view>
					<image class="dev-logo" src="/static/fan.png" mode=""></image>
				</view>
				<switch :checked="fan" @change="onFanSwitch" color="#3b82f6"/>
			</view>
<!-- 			水泵卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">水泵</view>
					<image class="dev-logo" src="/static/pump.png" mode=""></image>
				</view>
				<switch :checked="pump" @change="onPumpSwitch" color="#3b82f6"/>
			</view>
<!-- 			蜂鸣器卡片 -->
			<view class="dev-card">
				<view class="">
					<view class="dev-name">蜂鸣器</view>
					<image class="dev-logo" src="/static/beep.png" mode=""></image>
				</view>
				<switch :checked="beep" @change="onBeepSwitch" color="#3b82f6"/>
			</view>

		</view>
	</view>

</template>

<script>
	const {
		createCommonToken
	} = require('@/key.js')
	export default {
		data() {
			return {
				Temperature: '',
				Humidity: '',
				Light_lux: '',
				ccs_data: '',
				soil_humi: '',
				AnimalType: '',
				led: false,
				fan: false,
				pump: false,
				beep: false,
				token: '',
				lastControlTime: 0
			}
		},
		onShow() {
			this.fetchDevData();
			setInterval(() => {
				this.fetchDevData();
			}, 3000)
		},
		onLoad() {
			const params = { //用户秘钥
				access_key: '0Cc/Z9/tTIoHZ5Uo0zyl/zq1PXMZsk9kXRdLNs6X+ZKRnwGaE7dXOhSbutVkdCzm', //用户的accesskey
				version: '2022-05-01',
				user_id: '459983', //用户ID
			}
			this.token = createCommonToken(params);

		},
		methods: {
			// 获取设备数据
			fetchDevData() {
				// 【新增】当前时间
				let now = Date.now();
				
				// 【新增】如果距离上次手动控制小于 5000毫秒 (5秒)，则不刷新数据，防止按钮跳变
				if (now - this.lastControlTime < 2000) {
					console.log("处于控制冷却期，暂停刷新状态");
					return;
				}
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/query-device-property',
					method: 'GET',
					data: {
						product_id: '0718pmPIQe',
						device_name: 'device1'
					},
					header: {
						'authorization': this.token
					},
					success: (res) => {
						console.log(res.data);
						this.AnimalType = res.data.data[0].value;
						this.Humidity = res.data.data[1].value;
						this.Light_lux = res.data.data[2].value;
						this.Temperature = res.data.data[3].value;
						this.beep = res.data.data[4].value === 'true';
						this.ccs_data = res.data.data[5].value;
						this.fan = res.data.data[6].value === 'true';
						this.led = res.data.data[7].value === 'true';
						this.pump = res.data.data[8].value === 'true';
						this.soil_humi = res.data.data[9].value;
					}
				});
			},
			//设备下发控制
			setProperty(key, value) {
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property',
					method: 'POST',
					data: {
						product_id: '0718pmPIQe',
						device_name: 'device1',
						params: {
							[key]: value
						}
					},
					header: {
						authorization: this.token
					}
				})
			},
			onLedSwitch(e) {
				// 1. 记录当前操作时间，激活“冷却期”
				this.lastControlTime = Date.now();
				this.led = e.detail.value
				this.setProperty('led', this.led)
			},
			onFanSwitch(e) {
				this.lastControlTime = Date.now();
				this.fan = e.detail.value
				this.setProperty('fan', this.fan)
			},
			onPumpSwitch(e) {
				this.lastControlTime = Date.now();
				this.pump = e.detail.value
				this.setProperty('pump', this.pump)
			},
			onBeepSwitch(e) {
				this.lastControlTime = Date.now();
				this.beep = e.detail.value
				this.setProperty('beep', this.beep)
			},


		}
	}
</script>

<style>
	/* 页面整体背景 */
	.wrap{
		padding: 20rpx 0;
		background-color: #f5f5f5;/* 浅灰底色，为了衬托白卡片 */
		min-height: 100vh;
	}
	.dev-area {
		display: flex;
		justify-content: space-between;/* 两端对齐，中间留空 */
		flex-wrap: wrap;/* 允许换行 */
		padding: 0 30rpx;/* 左右留出边距 */
	}
	/* 单个卡片样式 */
	.dev-card {
		height: 160rpx;
		width: 48%;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 4rpx 15rpx rgba(0,0,0,0.05);/* 优化阴影，更柔和 */
		background-color: #fff;/* 必须设置白色背景 */
	}

	.dev-name {
		font-size: 26rpx;
		text-align: center;
		color: #6d6d6d;
		margin-bottom: 12rpx;
	}

	.dev-logo {
		width: 70rpx;
		height: 70rpx;
		display: block;
		
	}

	.dev-data {
		font-size: 44rpx;/* 数据字体大小 */
		color: #333;
		font-weight: 500;
	}
	
	.unit {
	  font-size: 24rpx;
	  color: #999;
	  margin-left: 4rpx;
	}
	
</style>