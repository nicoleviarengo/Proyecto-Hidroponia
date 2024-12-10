// Referencias a elementos
const harvestBtn = document.getElementById("harvestBtn");
const modal = document.getElementById("commentModal");
const closeModalBtn = document.querySelector(".close");
const saveCommentBtn = document.getElementById("saveCommentBtn");

// Mostrar el modal al hacer clic en "Cosechar"
harvestBtn.onclick = function(event) {
    event.preventDefault(); 
  modal.style.display = "block";
}

// Cerrar el modal al hacer clic en la "X"
closeModalBtn.onclick = function() {
  modal.style.display = "none";
}

// Cerrar el modal al hacer clic fuera de él
window.onclick = function(event) {
  if (event.target == modal) {
    modal.style.display = "none";
  }
}

// Guardar comentario en la base de datos
saveCommentBtn.onclick = function() {
  const comment = document.getElementById("harvestComment").value;

  // Validación: Asegurarse que no esté vacío
  if (comment.trim() === "") {
    alert("Por favor, ingresa un comentario.");
    return;
  }

  // Aquí llamamos a la API para guardar el comentario en la base de datos

  /*fetch('/guardarComentario', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      comentario: comment,
      // Aquí podrías agregar más datos como ID de la cosecha, fecha, etc.
    }),
  })
  .then(response => response.json())
  .then(data => {
    if (data.success) {
      alert("Comentario guardado con éxito");
      modal.style.display = "none"; // Cerrar el modal después de guardar
    } else {
      alert("Hubo un error al guardar el comentario.");
    }
  })
  .catch(error => {
    console.error('Error al guardar el comentario:', error);
    alert("Error de conexión.");
  });*/
}
