var stelem;
var droppedfile;

function handleDrop(e) {
  e.stopPropagation();
  e.preventDefault();

  var types = e.dataTransfer.types;
  var files = e.dataTransfer.files;
  var items = e.dataTransfer.items;

  if(types[0] == 'Files') {
    droppedfile = files[0];
    stelem.postMessage({msgtype: 'openurl',
                        url: 'dragndrop://' + files[0].name});
    return;
  }

  if(types[0] == 'text/uri-list') {
    items[0].getAsString(function(url) {
      stelem.postMessage({msgtype:'openurl', url: url});
    });
    return;
  }
}


function handleDragOver(e) {
  e.stopPropagation();
  e.preventDefault();
}

function handleMessage(e) {

  switch(e.data.msgtype) {
  case 'openfile':
    if(e.data.filename == droppedfile.name) {
      var m = {msgtype:'dndopenreply',
               reqid: e.data.reqid,
               size: droppedfile.size,
              };
      stelem.postMessage(m);
    } else {
      stelem.postMessage({msgtype:'dndopenreply',
                          reqid: e.data.reqid,
                          error: 1});
    }
    break;
  case 'readfile':
    var chunk = droppedfile.slice(e.data.fpos, e.data.fpos + e.data.size);
    var arrayBuffer;
    var fileReader = new FileReader();
    fileReader.onload = function() {
      arrayBuffer = this.result;
      stelem.postMessage({msgtype:'dndreadreply',
                          reqid: e.data.reqid,
                          buf: arrayBuffer});
    }
    fileReader.readAsArrayBuffer(chunk);
    break;

  case 'fsinfo':
    var fn = e.data.fs == 'cache' ? navigator.webkitTemporaryStorage :
      navigator.webkitPersistentStorage;

    fn.queryUsageAndQuota(function(used, size) {
      stelem.postMessage({msgtype:'rpcreply',
                          used: used,
                          size: size,
                          reqid: e.data.reqid});
    });
    break;

  case 'running':
    document.getElementById("loader").parentNode.removeChild(loader);
    break;
  }
}


function launchShowtime() {

  document.getElementById('showtimecontainer').innerHTML = '<embed id="showtime" src="showtime.nmf" type="application/x-pnacl"/>';

  stelem = document.getElementById('showtime')

  stelem.addEventListener('dragover', handleDragOver, false);
  stelem.addEventListener('drop', handleDrop, false);
  stelem.addEventListener('message', handleMessage, true);

}

navigator.webkitPersistentStorage.requestQuota(128*1024*1024, function(bytes) {
  console.log('Allocated '+bytes+' bytes of persistant storage.');
  launchShowtime();
  document.getElementById("showtime").focus();
}, function(e) {
  alert('Failed to allocate disk space, Showtime will not start')
});



document.addEventListener('visibilitychange', function() {
  stelem.postMessage({msgtype: document.hidden ? 'hidden' : 'visible'});
}, false);

