#include "TrainHUDWidget.h"
#include "WebBrowser.h"

void UTrainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UIBrowser)
	{
		const FString HTMLContent = TEXT(R"(
<!DOCTYPE html>
<html>
<head>
<style>
	body {
		margin: 0;
		padding: 0;
		overflow: hidden;
		font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
		color: white;
	}
	.dashboard {
		position: absolute;
		bottom: 50px;
		right: 50px;
		width: 400px;
		padding: 30px;
		background: rgba(20, 20, 30, 0.4);
		backdrop-filter: blur(15px);
		-webkit-backdrop-filter: blur(15px);
		border-radius: 20px;
		border: 1px solid rgba(255, 255, 255, 0.1);
		box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);
	}
	.metric {
		display: flex;
		justify-content: space-between;
		margin-bottom: 15px;
		font-size: 24px;
		border-bottom: 1px solid rgba(255,255,255,0.1);
		padding-bottom: 5px;
	}
	.value {
		font-weight: bold;
		color: #00ffcc;
		text-shadow: 0 0 10px rgba(0, 255, 204, 0.5);
	}
	h2 {
		margin-top: 0;
		color: #aaa;
		text-transform: uppercase;
		font-size: 14px;
		letter-spacing: 2px;
	}
</style>
</head>
<body>
	<div class="dashboard">
		<h2>Locomotive Telemetry</h2>
		<div class="metric"><span>Speed</span> <span class="value" id="speed">0 km/h</span></div>
		<div class="metric"><span>Brake Pipe</span> <span class="value" id="pipe">90 PSI</span></div>
		<div class="metric"><span>Cylinders</span> <span class="value" id="cylinder">0 PSI</span></div>
		<div class="metric" style="border:none;"><span>Throttle</span> <span class="value" id="throttle">0</span></div>
	</div>

	<script>
		function UpdateTelemetry(speed, pipe, cylinder, throttle) {
			document.getElementById('speed').innerText = speed + ' km/h';
			document.getElementById('pipe').innerText = pipe + ' PSI';
			document.getElementById('cylinder').innerText = cylinder + ' PSI';
			document.getElementById('throttle').innerText = 'Notch ' + throttle;
		}
	</script>
</body>
</html>
		)");

		UIBrowser->LoadString(HTMLContent, TEXT("dummy://hud"));
	}
}

void UTrainHUDWidget::UpdateHUDMetrics(float SpeedKmh, float PipePSI, float CylinderPSI, float Throttle)
{
	if (UIBrowser)
	{
		int32 Speed = FMath::RoundToInt(SpeedKmh);
		int32 Pipe = FMath::RoundToInt(PipePSI);
		int32 Cyl = FMath::RoundToInt(CylinderPSI);
		int32 Throt = FMath::RoundToInt(Throttle);

		FString JSCommand = FString::Printf(TEXT("UpdateTelemetry(%d, %d, %d, %d);"), Speed, Pipe, Cyl, Throt);
		UIBrowser->ExecuteJavascript(JSCommand);
	}
}
