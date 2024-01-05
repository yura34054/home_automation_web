import plotly.express as px
from django.shortcuts import render
from django.utils import timezone

from app.internal.models import SensorReading


def co2_chart(request):
    co2 = SensorReading.objects.filter(created_on__gte=timezone.now() - timezone.timedelta(hours=2))

    fig = px.line(
        x=[c.created_on for c in co2],
        y=[c.carbon_dioxide for c in co2],
        title="CO2 PPM",
        labels={"x": "Time", "y": "CO2 PPM"},
    )

    fig.update_layout(title={"font_size": 24, "xanchor": "center", "x": 0.5})
    html_chart = fig.to_html()
    context = {"chart": html_chart}
    return render(request, "chart.html", context)
