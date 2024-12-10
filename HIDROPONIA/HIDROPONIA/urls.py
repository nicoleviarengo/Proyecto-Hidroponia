"""
URL configuration for HIDROPONIA project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/5.1/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path, include
from cultivos import views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('cultivos/', include('cultivos.urls')),
    path('miscultivos/', views.guardar_cultivo, name='guardar_cultivo'),
    path('miscultivos/',views.guardar_parametros, name='guardar_parametros'),
    path('cultivos/', views.mostrar_fecha, name='mostrar_fecha'),
    #path('cultivos/', views.comentario, name='comentario')
]
