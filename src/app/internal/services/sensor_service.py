from sensirion_gas_index_algorithm.nox_algorithm import NoxAlgorithm
from sensirion_gas_index_algorithm.voc_algorithm import VocAlgorithm

from app.internal.models import Sensor, SensorReading

voc_algorithm = VocAlgorithm()
nox_algorithm = NoxAlgorithm()


def create_sensor_reading(
    controller_name: str, api_key: str, temperature, humidity, carbon_dioxide, raw_voc_data: [int], raw_nox_data: [int]
) -> bool:
    """Add new reading to the database"""

    sensor = Sensor.objects.get(controller_name=controller_name)
    if sensor.api_key != api_key:
        return False

    if carbon_dioxide == 0 | raw_voc_data[0] == 0:
        return False

    for s_voc_raw, s_nox_raw in zip(raw_voc_data, raw_nox_data):
        voc_index = voc_algorithm.process(s_voc_raw)
        nox_index = nox_algorithm.process(s_nox_raw)

    SensorReading.objects.create(
        sensor=sensor,
        temperature=temperature,
        humidity=humidity,
        carbon_dioxide=carbon_dioxide,
        voc_raw=round(sum(raw_voc_data) / len(raw_voc_data), 2),
        nox_raw=round(sum(raw_nox_data) / len(raw_nox_data), 2),
        voc_index=voc_index,
        nox_index=nox_index,
    )

    return True
