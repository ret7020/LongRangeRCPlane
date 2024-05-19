document.addEventListener("DOMContentLoaded", () => {
    let getVideoButton = document.getElementById("getVideo");
    let video = document.getElementById("cameraVideo");
    let mediaDevices = navigator.mediaDevices;
    video.muted = true;
    getVideoButton.addEventListener("click", () => {
        mediaDevices
            .getUserMedia({
                video: true,
                audio: false,
            })
            .then((stream) => {
                // Changing the source of video to current stream.
                video.srcObject = stream;
                video.addEventListener("loadedmetadata", () => {
                    video.play();
                });
            })
            .catch(alert);
    });
});