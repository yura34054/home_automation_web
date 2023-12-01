from app.internal.models import Sensor, SensorReading


def crate_sensor_reading(name: str, api_key: str, temperature, humidity, carbon_dioxide) -> bool:
    """Add new reading to the database"""

    sensor = Sensor.objects.get(name=name)
    if sensor.api_key != api_key:
        return False

    SensorReading.objects.create(
        sensor=sensor,
        temperature=temperature,
        humidity=humidity,
        carbon_dioxide=carbon_dioxide,
    )

    return True
