const client = new Paho.MQTT.Client(
  "broker.emqx.io",
  8084,
  "web_" + Math.random(),
);

const TOPICO_SENSORES = "fran/sensores";
const TOPICO_ALERTA = "fran/alerta";

// Configuração do CallMeBot (WhatsApp)
const WHATSAPP_TELEFONE = import.meta.env.WTEL;
const WHATSAPP_APIKEY = import.meta.env.WAPIKEY;

if ("Notification" in window && Notification.permission === "default") {
  Notification.requestPermission();
}

function enviarWhatsApp(texto) {
  const url = `https://api.callmebot.com/whatsapp.php?phone=${WHATSAPP_TELEFONE}&text=${encodeURIComponent(texto)}&apikey=${WHATSAPP_APIKEY}`;
  return fetch(url);
}

client.onMessageArrived = function (message) {
  if (message.destinationName === TOPICO_ALERTA) {
    if ("Notification" in window && Notification.permission === "granted") {
      new Notification("Alerta: Ponte Interditada", {
        body: "Uma ponte foi marcada como interditada. Evite a rota.",
      });
    } else {
      alert("Alerta: Ponte Interditada! (ative as notificações do navegador para receber isso automaticamente da próxima vez)");
    }
    return;
  }

  const data = JSON.parse(message.payloadString);

  document.getElementById("pilar1").textContent = data.angulo1.toFixed(1) + " °";
  document.getElementById("pilar2").textContent = data.angulo2.toFixed(1) + " °";

  const statusPonte = document.getElementById("status-ponte");
  const statusCancela = document.getElementById("status-cancela");
  const alertaMovimentacao = document.getElementById("alerta-movimentacao");

  if (data.risco) {
    statusPonte.textContent = "Risco";
    statusPonte.className = "sensor-value alerta";
    alertaMovimentacao.style.display = "block";
  } else {
    statusPonte.textContent = "Normal";
    statusPonte.className = "sensor-value ok";
    alertaMovimentacao.style.display = "none";
  }

  statusCancela.textContent = data.cancela === "fechada" ? "Fechada" : "Aberta";
  statusCancela.className = data.cancela === "fechada" ? "sensor-value alerta" : "sensor-value ok";
};

function conectar() {
  client.connect({
    useSSL: true,
    onSuccess: () => {
      const status = document.getElementById("status");
      status.textContent = "Conectado";
      status.className = "has-text-centered status-ok";

      client.subscribe(TOPICO_SENSORES);
      client.subscribe(TOPICO_ALERTA);
    },
    onFailure: () => {
      const status = document.getElementById("status");
      status.textContent = "Reconectando...";
      status.className = "has-text-centered status-erro";
      setTimeout(conectar, 2000);
    },
  });
}
conectar();

document.getElementById("btn-alerta").addEventListener("click", () => {
  const botao = document.getElementById("btn-alerta");
  botao.disabled = true;
  botao.textContent = "Enviando alerta...";

  if (!("Notification" in window)) {
    alert("Seu navegador não suporta notificações. O alerta será enviado, mas você não vai vê-lo aqui.");
  } else if (Notification.permission === "denied") {
    alert("As notificações estão bloqueadas no seu navegador. Ative-as nas configurações do site para receber este alerta.");
  } else if (Notification.permission === "default") {
    Notification.requestPermission();
  }

  const mensagem = new Paho.MQTT.Message("interditada");
  mensagem.destinationName = TOPICO_ALERTA;
  client.send(mensagem);

  enviarWhatsApp("Alerta: Ponte Interditada! Sistema de Monitoramento - " + new Date().toLocaleString("pt-BR"))
    .then(() => {
      botao.textContent = "Alerta enviado ✓";
    })
    .catch(() => {
      botao.textContent = "Falha ao enviar";
      alert("Não foi possível enviar a mensagem via WhatsApp. Verifique sua conexão ou a apikey.");
    })
    .finally(() => {
      setTimeout(() => {
        botao.disabled = false;
        botao.textContent = "Enviar alerta de ponte interditada";
      }, 5000);
    });
});