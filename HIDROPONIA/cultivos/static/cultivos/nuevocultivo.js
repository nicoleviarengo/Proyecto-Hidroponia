const modal = document.getElementById('cultivoModal');
const btnNuevoCultivo = document.getElementById('nuevoCultivoBtn');
const spanClose = document.getElementsByClassName('close')[0];

btnNuevoCultivo.onclick = function() {
  modal.style.display = 'block';
}

spanClose.onclick = function() {
  modal.style.display = 'none';
}

window.onclick = function(event) {
  if (event.target == modal) {
    modal.style.display = 'none';
  }
}

// logica del formulario
document.getElementById('cultivoForm').addEventListener('submit', function(event) {
  event.preventDefault();

  const nombre = document.getElementById('nombre').value;
  const tipo = document.getElementById('tipo').value;
  const cuando = document.getElementById('cuando').value;
  const cuidados = document.getElementById('cuidados').value;
  const comentario = document.getElementById('comentario').value;

  console.log('Nuevo Cultivo:', { nombre, tipo, cuando, cuidados, comentario });

  modal.style.display = 'none';

  this.reset();
});
