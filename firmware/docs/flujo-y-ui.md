🌤️ Flujo de interacción — YUDI
Documentación oficial del sistema de juego

Versión: completa, limpia y con estructura técnica

🧭 1. Estructura general del sistema

YUDI es un juguete interactivo con:

Pantalla LCD 16x2 I2C (0x27)

DFPlayer Mini (audio)

12 botones (modo, respuestas, confirmación)

Carita animada personalizada

Máquina de estados completa para flujo de juego

El sistema se divide en:

Encendido / Presentación

Selector de modo

Modo Cortesía

Modo Emociones

Interrupción (botón NO)

Cierre general

⚡ 2. Encendido
2.1 Secuencia de inicio

Animación TEAYUDA — 4 segundos

TEAYUDA fijo — 3 segundos

Carita parpadeando — 2 ciclos (~4 s)

2.2 Presentación

LCD:

Soy Yudi,
la nube


DFPlayer: sonido 0001

2.3 Mensaje de inicio

LCD:

ES HORA DE
JUGAR!


Audio: 0003

🎮 3. Selector de modo

LCD:

Elige un modo


Espera 15 segundos por input:

Modo	Botón	Pin
Modo Cortesía	Botón modo cortesia	A3
Modo Emociones	Botón modo emociones	A2

Si no hay respuesta → Cierre general

Audio correspondiente: 0047

🤝 4. Modo CORTESÍA

El modo consta de 5 preguntas sociales.
Cada pregunta tiene:

Texto en LCD

Audio correspondiente

Una respuesta correcta

Manejo de errores:

Primer error → repetir la pregunta

Segundo error → repetición guiada

Sonidos:

0057 → error

0058 → recompensa

🟦 Pregunta 1
Un amigo
te dice hola


Pregunta: ¿Qué dices?
Respuesta correcta: HOLA (pin digital 5)
Audio: 0005 / 0006 / 0007
Éxito: Gran trabajo!
Error:

Probemos
de nuevo


Repetición guiada:

Repite conmigo:
Hola

🟦 Pregunta 2
Te regalan
un dulce


Respuesta: GRACIAS (pin 6)
Audio: 0012 / 0013 / 0014
Éxito: Bravo!
Repetición guiada: Gracias

🟦 Pregunta 3
Alguien te
dice gracias


Respuesta: DE NADA (pin 8)
Audio: 0018 / 0019

🟦 Pregunta 4
Al pedir algo.
Se dice:


Respuesta: POR FAVOR (pin 7)
Audio: 0021 / 0022

🟦 Pregunta 5
El adiós mágico
es:


Respuesta: HASTA LUEGO (pin 9)
Audio: 0048 / 0049

🟨 Cierre del modo Cortesía

Mensajes en orden:

Gracias por
jugar conmigo

Eres genial!

Modo Cortesia
Terminado!

Quieres jugar
de nuevo?

Si SÍ (A0)
Perfecto,
vamos!


Audio: 0025

→ Regresa a “Elige un modo”

Si NO (A1) o timeout
Esta bien,
jugamos luego


→ Cierre general

❤️ 5. Modo EMOCIONES

Incluye 5 preguntas sobre reconocimiento emocional.

🟥 Pregunta 1: "¿Cómo te sientes ahora?"
Como te
sientes ahora?


Botones:

Emoción	Pin
FELIZ	D3
TRISTE	D4
ENOJADO	D2

Respuestas:

Feliz

Que alegria!


Audio: 0030

Triste

Todo mejorará,
Estoy contigo


Audio: 0032

Enojado

Oh, respira
conmigo


Luego:

Inhala
Exhala


Audios: 0059 / 0060

🟥 Pregunta 2
Un amigo
esta llorando

Sabes que
emocion siente?


Respuesta: TRISTE
Éxito:

Muy bien!
Lo reconociste

🟥 Pregunta 3
Si estas feliz
puedes decir...


Respuesta: FELIZ
Éxito:

Genial!
Sonríe y comparte

🟥 Pregunta 4
Si estas triste
puedes decir…


Resultado: TRISTE

🟥 Pregunta 5
Si estas enojado
puedes decir...


Respuesta: ENOJADO

Muy bien!
El enojo se va
poquito a poquito

🟨 Cierre del modo Emociones

Mensajes finales:

Lo lograste!
Eres valiente

Recuerda, todas
las emociones

son importantes

Modo Emociones
Terminado!

Quieres jugar
de nuevo?

Si SÍ (A0)
Perfecto,
vamos!

Si NO
Esta bien,
jugamos luego


→ Cierre general

🛑 6. Interrupción del juego (botón NO)

Si se presiona botón NO (A1) durante cualquier pregunta:

Quieres terminar
el juego?


Si luego presiona SÍ (A0):

Esta bien,
te espero luego


→ Cierre general

Si presiona NO (A1):

Que bien!
Sigamos jugando!


→ Continúa el juego

🌙 7. Cierre general

Pantalla en reposo:

Estoy aqui
cuando quieras!!


Cuando presiona cualquier botón:

Quieres jugar
de nuevo?


Si SÍ:

Perfecto,
vamos!


→ Selección de modo

Si NO:

Esta bien,
jugamos luego


Regresa a pantalla inicial de cierre.

🧩 8. Mapa de pines
Botones emocionales

D2 → Molesto / Enojado

D3 → Feliz

D4 → Triste

Botones de cortesía

D5 → Hola

D6 → Gracias

D7 → Por favor

D8 → De nada

D9 → Hasta luego

DFPlayer

D10 → Tx

D11 → Rx

Opciones del usuario

A0 → Sí

A1 → No

A2 → Modo emociones

A3 → Modo cortesia

🔉 9. Reglas de audio

El DFPlayer usa los siguientes IDs:

(Se listan completos tal como los definiste: 0001 a 0065)

(Conservarás esta sección idéntica.)

Volumen del DFPlayer:

Al iniciar SIEMPRE:

myDFPlayer.volume(10);


Aunque se apague o reinicie.

🖥️ 10. Pantalla sin parpadeo

Usar una función de actualización parcial:

void printLCD(const String &l1, const String &l2);


No usar lcd.clear() excepto en inicio o cambio mayor

Limpiar líneas con espacios

Carita parpadea reescribiendo SOLO los ojos

🎭 11. Carita animada (CGRAM)

Usar la carita exactamente como en el código:

eyeOpen

eyeClosed

mouthHappyLeft

mouthHappyRight

Funciones:

drawFace(true/false)

drawEyes(true/false)

🧠 12. Lógica de preguntas (regla de errores)

Para cada pregunta:

Primer error → Repetir pregunta

Segundo error → Mostrar repetición guiada + audio

Seguir a la siguiente pregunta
