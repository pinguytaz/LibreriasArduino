/* ******* Fichero Pagina.html comprimido
   Pagina.html.gz, Size: xxx
   Herramienta WEB https://gchq.github.io/CyberChef/ 
         Comprime GZIP (Dynamic Huffman Coding)
         a Hex   Delimitador 0x
         Split   Delimitador 0x  que ponga coma,0x
* ***********************/
/* #define Pagina_html_gz_len 1220
const uint8_t Pagina_html_gz[] = { 0x1f,0x8b};
*/

// ******* Fichero Pagina.html Opcion descomprimida
//  Se guarda en Flash "PROGMEN" y esta entre los literales (R) pagprincipal
const char Pagina_html[] PROGMEM = R"pagprincipal(
<!doctype html>
<html>
   <head>
     <meta charset="utf-8">
     <meta name="viewport" content="width=device-width,initial-scale=1">
     <style>
        body 
        {
          font-family: Arial,Helvetica,sans-serif;
          background: #080858;
          color: #EFEFEF;
          font-size: 16px
        }
        #contenido 
        {
          display: flex;
          flex-wrap: wrap;
          align-items: stretch
        }
        .input-group 
        {
          display: flex;
          flex-wrap: nowrap;
          line-height: 22px;
          margin: 5px 0
        }
        .input-group>label 
        {
          display: inline-block;
          padding-right: 9px;
          min-width: 47%
        }
        .input-group input,.input-group select 
        {
          flex-grow: 1
        }
     </style>
   </head>  
   <title>Parametros para ESP32 con OV2460</title>
   <body>
     <section class="principal">
       <div id="contenido">
         <div id="barramenu">
           <button id="cambiaflash">Flash</button>

           <div class="input-group" id="framesize-group">
             <label for="framesize">Resolution</label>
            <select id="framesize" class="default-action">
               <option value="4">240x240</option>
               <option value="5">QVGA(320x240)</option>
               <option value="6">CIF(400x296)</option>
               <option value="8">VGA(640x480)</option>
               <option value="9">SVGA(800x600)</option>
               <option value="10">XGA(1024x768)</option>
               <option value="12">SXGA(1280x1024)</option>
               <option value="13">UXGA(1600x1200)</option>
             </select>
           </div>
         
           <div class="input-group" id="quality-group">
              <label for="quality">Calidad</label> <input id="calidad" type="number" min="0" max="63" size="2" value="10" class="default-action">
           </div>

           <div class="input-group" id="brightness-group">
             <label for="brightness">Brillo</label> <input id="brillo" type="number" min="-2" max="2" size="2" value="0" class="default-action">
            </div>
         
           <div class="input-group" id="contrast-group">
             <label for="contrast">Contraste</label> <input id="contraste" type="number" min="-2" max="2" size="2" value="0" class="default-action">
           </div>
          
           <div class="input-group" id="saturation-group">
             <label for="saturation">Saturacion</label> <input id="saturacion" type="number" min="-2" max="2" size="2" value="0" class="default-action">          
           </div>
           
         </div>
           <figure>
            <figcaption> Streaming en tiempo real con camara ESP32-CAM  </figcaption>
            <img src="" id="laFoto">
         </figure>
       </div>
     </section>
<script>
   //  Variables globales
   var baseHost = document.location.origin
   var baseStream = "http://" + document.location.hostname + ":8081"

   // Controles a gestiona
   var botonFlash = document.getElementById('cambiaflash')
   var framesize = document.getElementById('framesize')
   var calidad = document.getElementById('calidad')
   var brillo = document.getElementById('brillo')
   var contraste = document.getElementById('contraste')
   var saturacion = document.getElementById('saturacion')
   

   // Evento que se ejecuta al cargar la pagina
   document.addEventListener('DOMContentLoaded', function (event) 
   {
     // Se leen los valores de configuracion de la camara para actualizar campos
     const query = baseHost +"/status"
     fetch(query).then(function (response) 
     {
       return response.json()
     }).then(function (state) 
     {
       document.querySelectorAll('.default-action').forEach(el => 
       {
         el.value = state[el.id]
       })
     })

     // Activamos streaming
     var srcfoto = baseStream + "/stream"
     document.getElementById('laFoto').src = srcfoto
    })   // Fin del Evento


   // Funcionamiento de Boton Flash
   botonFlash.onclick = () => 
    {
       const noFlash = botonFlash.innerHTML === 'Flash'
       if (noFlash) startFlash()
       else stopFlash()
    }
   function stopFlash() 
   {
     botonFlash.innerHTML = 'Flash'
     var query = baseHost +"/flash?activa=0"
     fetch(query).then(response => 
     {
       console.log(`request to ${query} finalizado, status: ${response.status}`)
     })
    }
    function startFlash()
    {
       botonFlash.innerHTML = 'Apaga Flash'
       var query = baseHost +"/flash?activa=1"
       fetch(query).then(response => 
       {
         console.log(`request to ${query} finalizado, status: ${response.status}`)
       })
    }
    
   // Cambio de de resolucion
   framesize.onchange = () => 
   {  
     var query = baseHost +"/control?var=framesize&val="+ framesize.value
     fetch(query).then(response => 
     {
       console.log(`request to ${query} finalizado, status: ${response.status}`)
     })
   }

   // Cambios de propiedades
   calidad.onchange = function()
   {
     var valor = calidad.value
     var query = baseHost +"/control?var=calidad&val="+ valor
     fetch(query).then(response => 
     {
       console.log(`request to ${query} finalizado, status: ${response.status}`)
     })
   }
   brillo.onchange = () => 
   {
     var valor = brillo.value
     var query = baseHost +"/control?var=brillo&val="+ valor
     fetch(query).then(response => 
     {
       console.log(`request to ${query} finalizado, status: ${response.status}`)
     })
   }
   contraste.onchange = () => 
   {
     var valor = contraste.value
     var query = baseHost +"/control?var=contraste&val="+ valor
     fetch(query).then(response => 
     {
       console.log(`request to ${query} finalizado, status: ${response.status}`)
     })
   }
   saturacion.onchange = () => 
   {
     var valor = saturacion.value
     var query = baseHost +"/control?var=saturacion&val="+ valor
     fetch(query).then(response => 
     {
        console.log(`request to ${query} finalizado, status: ${response.status}`)
     })
   }

</script>
  </body>
</html>

)pagprincipal";
