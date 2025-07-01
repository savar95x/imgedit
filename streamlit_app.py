import streamlit as st
from PIL import Image, ImageEnhance
import io

st.set_page_config(layout="wide")
st.title("Image Editor")
default_image = "theoffice.jpg"

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
    image = Image.open(uploaded_file)
else:
    image = Image.open(default_image)

edited_image = image
edited_image = ImageEnhance.Color(edited_image).enhance(1 - grayscale)
edited_image = ImageEnhance.Brightness(edited_image).enhance(brightness)
edited_image = ImageEnhance.Contrast(edited_image).enhance(contrast)

st.subheader("Edited Image")
st.image(edited_image)
