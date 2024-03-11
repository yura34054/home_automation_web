from app.internal.models import Sensor, SensorReading


def create_sensor_reading(
        controller_name: str,
        api_key: str,
        temperature: float,
        humidity: float,
        carbon_dioxide: float,
        voc_index: int,
        nox_index: int,
        pm1_0: float,
        pm2_5: float,
        pm10: float
):
    sensor = Sensor.objects.get(name=controller_name)
    if sensor.api_key != api_key:
        return False

    SensorReading.objects.create(
        sensor=sensor,
        temperature=temperature,
        humidity=humidity,
        carbon_dioxide=carbon_dioxide,
        voc_index=voc_index,
        nox_index=nox_index,
        pm1_0=pm1_0,
        pm2_5=pm2_5,
        pm10=pm10,
    )
