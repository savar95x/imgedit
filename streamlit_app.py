import streamlit as st
import subprocess
import io

st.set_page_config(layout="wide")
st.title("Image Editor")
default_image = "imgs/berries.jpg"

with st.sidebar:
    uploaded_file = st.file_uploader("", type=["jpg", "jpeg", "png"])
    if uploaded_file:
        st.image(uploaded_file)
    else:
        st.image(default_image)
    contrast = st.slider("contrast", 0.5, 2.0, 1.0, 0.05)
    brightness = st.slider("brightness", 0.5, 2.0, 1.0, 0.05)
    grayscale = st.slider("grayscale", 0.0, 1.0, 0.5, 0.05)

st.write(
        "Open the Sidebar to upload or edit image."
)

if uploaded_file is not None:
    image = uploaded_file
else:
    image = default_image

commands = [
    ["bin/imgedit", "--grayscale", str(grayscale), image, "imgs/temp1.jpg"],
    ["bin/imgedit", "--brightness", str(brightness), "imgs/temp1.jpg", "imgs/temp2.jpg"],
    ["bin/imgedit", "--contrast", str(contrast), "imgs/temp2.jpg", "imgs/out.jpg"],
]

for cmd in commands:
    try:
        result = subprocess.run(
            cmd,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
    except subprocess.CalledProcessError as e:
        st.error(f"❌ Failed: {' '.join(cmd)}\nError: {e.stderr}")
        break  # Stop if any command fails

st.subheader("Edited Image")
st.image("imgs/out.jpg")
