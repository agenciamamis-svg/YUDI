# 🌥️ YUDI — Dispositivo TEA YUDA

**Firmware Arduino UNO** para el dispositivo educativo **TEA YUDA**, diseñado para fomentar la interacción social y emocional en niños dentro del Trastorno del Espectro Autista (TEA).  
El sistema guía al usuario mediante preguntas, reconocimiento de emociones y expresiones de cortesía, con retroalimentación visual en una pantalla **LCD I2C 16x2** y botones físicos de selección.

---

## 🎯 Objetivo
Promover el desarrollo de habilidades sociales y emocionales básicas a través del juego guiado con la voz y una interfaz tangible sencilla.

---

## ⚙️ Características principales
- Microcontrolador: **Arduino UNO**
- Interfaz: **Pantalla LCD I2C 16x2**
- Modos de juego independientes:
  - 🟢 **Modo Cortesía:** enseña expresiones sociales como “Hola”, “Gracias”, “Por favor”, “De nada”.
  - 💗 **Modo Emociones:** enseña reconocimiento y expresión de emociones (Feliz, Triste, Molesto).
- Botones físicos para cada emoción o palabra clave.
- Indicadores visuales en pantalla y animación de “carita feliz”.
- Secuencia interactiva con preguntas y refuerzos positivos.

---

## 🧩 Mapa de pines
Consulta el archivo [`hardware/wiring-map.md`](hardware/wiring-map.md) para el detalle de conexiones eléctricas y diagramas.

---

## 📚 Documentación adicional
- [`docs/flujo-y-ui.md`](docs/flujo-y-ui.md): estructura del flujo de interacción (modos, preguntas y retroalimentación).
- [`hardware/bill-of-materials.md`](hardware/bill-of-materials.md): lista de componentes usados.

---

## 🚀 Instrucciones de uso
1. Conecta el dispositivo y asegúrate de que el **switch principal** esté en *ON*.
2. Al encender, la pantalla mostrará la carita de Yudi y el mensaje “Elige un modo”.
3. Usa los botones **Modo Cortesía** o **Modo Emociones** para iniciar.
4. Responde presionando los botones correspondientes según las preguntas.
5. Al finalizar, Yudi mostrará mensajes de cierre y ofrecerá volver a jugar.

---

## 🛠️ Requerimientos
- Arduino IDE 2.x  
- Librerías necesarias:
  - `Wire.h`
  - `LiquidCrystal_I2C.h`

---

## 👩‍💻 Créditos
**Autora:** Agencia Mamis (|mamis-svg)  
**Proyecto:** Laboratorio de Innovación – TEA YUDA  
**Año:** 2025  

---

## 📄 Licencia
Este proyecto está bajo licencia **MIT** — ver el archivo [LICE]()
