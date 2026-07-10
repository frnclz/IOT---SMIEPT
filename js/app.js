// Cria um cliente MQTT WebSocket para comunicação com o broker
const client = new Paho.MQTT.Client(
  "broker.emqx.io",
  8084,
  "web_" + Math.random(),
);

// Executado sempre que uma nova mensagem MQTT é recebida
const TOPICO_SENSORES = "fran/sensores";
const TOPICO_ALERTA = "fran/alerta";

if ("Notification" in window && Notification.permission === "default") {
  Notification.requestPermission();
}

client.onMessageArrived = function (message) {
  // Alerta manual (botão)
  if (message.destinationName === TOPICO_ALERTA) {
    if ("Notification" in window && Notification.permission === "granted") {
      new Notification("Alerta: Ponte Interditada", {
        body: "Uma ponte foi marcada como interditada. Evite a rota.",
      });
    }
    return;
  }

  // Dados dos sensores (automático)
  const data = JSON.parse(message.payloadString);

  document.getElementById("pilar1").textContent =
    data.angulo1.toFixed(1) + " °";
  document.getElementById("pilar2").textContent =
    data.angulo2.toFixed(1) + " °";

  const statusPonte = document.getElementById("status-ponte");
  const statusCancela = document.getElementById("status-cancela");
  const alertaMovimentacao = document.getElementById("alerta-movimentacao");

  const botaoAlerta = document.getElementById("btn-alerta");

  if (data.risco) {
    statusPonte.textContent = "Risco";
    statusPonte.className = "sensor-value alerta";
    alertaMovimentacao.style.display = "block";

    // Habilita o botão
    botaoAlerta.disabled = false;
  } else {
    statusPonte.textContent = "Normal";
    statusPonte.className = "sensor-value ok";
    alertaMovimentacao.style.display = "none";

    // Desabilita o botão
    botaoAlerta.disabled = true;
  }

  statusCancela.textContent = data.cancela === "fechada" ? "Fechada" : "Aberta";
  statusCancela.className =
    data.cancela === "fechada" ? "sensor-value alerta" : "sensor-value ok";
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
  if (!("Notification" in window)) {
    alert(
      "Seu navegador não suporta notificações. O alerta será enviado, mas você não vai vê-lo aqui.",
    );
  } else if (Notification.permission === "denied") {
    alert(
      "As notificações estão bloqueadas no seu navegador. Ative-as nas configurações do site para receber este alerta.",
    );
  } else if (Notification.permission === "default") {
    alert("Ative as notificações do navegador para receber este alerta.");
    Notification.requestPermission();
  }

  const mensagem = new Paho.MQTT.Message("interditada");
  mensagem.destinationName = TOPICO_ALERTA;
  client.send(mensagem);
});
