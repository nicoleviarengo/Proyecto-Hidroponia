from django.shortcuts import render
from .models import Cultivo, Resumen, Parametros
from django.http import JsonResponse
from django.shortcuts import get_object_or_404, redirect
import requests
from datetime import datetime
from django.core.paginator import Paginator
from django.utils import timezone

def mostrar_fecha(request):
    resumen = Resumen.objects.all()
    return render(request, 'web.html', {'resumen':resumen})

def cometario(request):
    resumen = Resumen.objects.all() 
    return render(request, 'web.html', {'cultivos': resumen})

def web(request):
    cultivos = Cultivo.objects.all() 
    resumen = Resumen.objects.all()
    paginator = Paginator(resumen, 3) ##agregado

    page_number = request.GET.get('page')##agregado
    page_obj = paginator.get_page(page_number)##agegado
    #API OpenWeatherMap
    city = '2400,ar'
    url = 'https://api.openweathermap.org/data/2.5/weather?zip={}&appid=9020f6a47613a51db84d74f2a28ed22d&units=metric'
    temperatura = []
    get_weather = requests.get(url.format(city)).json()
    weather = {
        'temp': round(get_weather['main']['temp']),
        'icon': get_weather['weather'][0]['icon'],
    }
    temperatura.append(weather)
    print(temperatura)
    
    fechaa = request.GET.get('fecha')
    hora = request.GET.get('hora')
    # Verifica que ambos campos hayan sido seleccionados
    if fechaa and hora:
        # Convierte fecha y hora a formato datetime
        fecha = f"{fechaa} {hora}"
        try:
            fecha = timezone.datetime.strptime(fecha, "%Y-%m-%d %H:%M")
            fecha = timezone.make_aware(fecha)
            
            # Filtra los registros según la fecha y hora seleccionadas
            datos_filtrados = Resumen.objects.filter(fecha=fecha)
        except ValueError:
            datos_filtrados = []
    else:
        # Si no se seleccionó una fecha y hora, muestra todos los datos o maneja como prefieras
        datos_filtrados = Resumen.objects.all()
    context = {
        'temperatura':temperatura,
        'cultivos':cultivos,
        'resumen': resumen,
        'page_obj': page_obj,
        'datos_filtrados': datos_filtrados,
        'fecha_seleccionada': fechaa,
        'hora_seleccionada': hora,
        }
    # Renderiza la plantilla con los datos filtrados
    return render(request, 'web.html', context )

def mis_cultivos(request):
    cultivos = Cultivo.objects.all() 
    return render(request, 'miscultivos.html', {'cultivos': cultivos})

def guardar_cultivo(request):
    if request.method == 'POST':
        try:
            nombre = request.POST.get('nombre')
            tipo = request.POST.get('tipo')
            cuandoSeCultiva = request.POST.get('cuando')
            cuidados = request.POST.get('cuidados')
            comentarios = request.POST.get('comentarios')

            if not all([nombre, tipo, cuandoSeCultiva, cuidados, comentarios]):
                return JsonResponse({'error': 'Faltan datos'}, status=400)

            cultivo = Cultivo(
                nombre=nombre,
                tipo=tipo,
                cuandoSeCultiva=cuandoSeCultiva,
                cuidados=cuidados,
                comentarios=comentarios
            )
            cultivo.save()
            return JsonResponse({'message': 'Cultivo guardado exitosamente'}, status=200)
        except Exception as e:
            # Maneja la excepción y devuelve un mensaje de error
            return JsonResponse({'error': f'Ocurrio un error: {str(e)}'}, status=500)   
    return JsonResponse({'error': 'Método no permitido'}, status=405)

def actualizar_cultivo(request, id):
    if request.method == 'POST':
        cultivo = get_object_or_404(Cultivo, id=id)
        cultivo.nombre = request.POST['nombre']
        cultivo.tipo = request.POST['tipo']
        cultivo.cuandoSeCultiva = request.POST['cuando']
        cultivo.cuidados = request.POST['cuidados']
        cultivo.comentarios = request.POST['comentarios']
        cultivo.save()
        return redirect('mis_cultivos')

def eliminar_cultivo(request, id):
    if request.method == 'POST':
        cultivo = get_object_or_404(Cultivo, id=id)
        cultivo.delete()
        return redirect('mis_cultivos')

def guardar_parametros(request, id):
    if request.method == 'POST':
        try:
            luminiscencia_min = request.POST.get('luminiscencia_min')
            luminiscencia_max = request.POST.get('luminiscencia_max')
            humedadSuelo_min = request.POST.get('humedadSuelo_min')
            humedadSuelo_max = request.POST.get('humedadSuelo_max')
            temperaturaAmbiente_min = request.POST.get('temperaturaAmbiente_min')
            temperaturaAmbiente_max = request.POST.get('temperaturaAmbiente_min')
            humedadAmbiente_min = request.POST.get('humedadAmbiente_min')
            humedadAmbiente_max = request.POST.get('humedadAmbiente_max')

            if not all([luminiscencia_min,luminiscencia_max,humedadSuelo_min,humedadSuelo_max, temperaturaAmbiente_min, temperaturaAmbiente_max, humedadAmbiente_min, humedadAmbiente_max]):
                return JsonResponse({'error': 'Faltan datos'}, status=400)

            parametros = Parametros(
                luminiscencia_min = luminiscencia_min,
                luminiscencia_max = luminiscencia_max,
                humedadSuelo_min = humedadSuelo_min,
                humedadSuelo_max = humedadSuelo_max,
                temperaturaAmbiente_min = temperaturaAmbiente_min,
                temperaturaAmbiente_max = temperaturaAmbiente_max,
                humedadAmbiente_min = humedadAmbiente_min,
                humedadAmbiente_max = humedadAmbiente_max,
            )
            parametros.save()
            return JsonResponse({'message': 'Parametros guardados exitosamente'}, status=200)
        except Exception as e:
            # Maneja la excepción y devuelve un mensaje de error
            return JsonResponse({'error': f'Ocurrio un error: {str(e)}'}, status=500)   
    return JsonResponse({'error': 'Método no permitido'}, status=405)


