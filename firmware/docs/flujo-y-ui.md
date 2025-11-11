# 🧭 Flujo de interacción — TEA YUDA

## 🌟 Estructura general
1. **Inicio / Encendido**
   - Se muestra animación de carita.
   - Pantalla: “Elige un modo”.
2. **Selección de modo**
   - Si se presiona **Modo Cortesía**, inicia preguntas sociales.
   - Si se presiona **Modo Emociones**, inicia reconocimiento emocional.

---

## 🟢 Modo Cortesía
| Etapa | Mensaje | Respuesta esperada | Retroalimentación |
|--------|----------|--------------------|--------------------|
| 1 | “Un amigo te dice hola. ¿Qué respondes?” | HOLA | “¡Gran trabajo! Repite conmigo: Hola” |
| 2 | “Te regalan un dulce. ¿Qué se dice?” | GRACIAS | “¡Bravo! Repite conmigo: Gracias” |
| 3 | “Alguien te dice gracias. ¿Qué respondes?” | DE NADA | “¡Súper bien! Repite conmigo: De nada” |
| 4 | “Al pedir algo, se dice...” | POR FAVOR | “¡Súper bien! Repite conmigo: Por favor” |

---

## 💗 Modo Emociones
| Etapa | Mensaje | Respuesta esperada | Retroalimentación |
|--------|----------|--------------------|--------------------|
| 1 | “¿Cómo te sientes ahora?” | FELIZ / TRISTE / MOLESTO | Muestra emoción correspondiente |
| 2 | “Un amigo está llorando. ¿Qué emoción siente?” | TRISTE | “Muy bien, lo reconociste” |
| 3 | “Si estás feliz, puedes decir...” | FELIZ | “Excelente, comparte tu alegría” |
| 4 | “Si estás triste, puedes decir...” | TRISTE | “Muy bien, así pides ayuda” |
| 5 | “Si estás enojado, puedes...” | MOLESTO | “Muy bien, respira conmigo (inhala/exhala)” |

---

## 🔁 Cierre y reinicio
Al completar un modo:
- “¿Quieres jugar de nuevo?”
  - **Sí (A0):** vuelve al menú principal.
  - **No (A1):** muestra “Está bien, jugamos luego” y vuelve al modo espera con la carita parpadeante.

---

## 💡 Indicaciones visuales
- Carita feliz: aparece en cada inicio y cierre.  
- Ojos parpadean cada ~1.2 segundos.  
- Mensajes aparecen centrados y breves para fácil lectura.
