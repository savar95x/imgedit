import streamlit as st
from PIL import Image, ImageEnhance
import io

st.set_page_config(layout="wide")
st.title("Image Editor")
default_image = "theoffice.jpg"

with st.sidebar:
    st.header("Settings")
    contrast = st.slider("contrast", 0.5, 2.0, 1.0, 0.1)
    brightness = st.slider("brightness", 0.5, 2.0, 1.0, 0.1)
    grayscale = st.slider("grayscale", 0.0, 1.0, 0.5, 0.1)

uploaded_file = st.file_uploader("", type=["jpg", "jpeg", "png"])

st.write(
        "Open the Sidebar to edit the image."
)
# load image
if uploaded_file is not None:
    image = Image.open(uploaded_file)
else:
    image = Image.open(default_image)

edited_image = image
edited_image = ImageEnhance.Color(edited_image).enhance(1 - grayscale)
edited_image = ImageEnhance.Brightness(edited_image).enhance(brightness)
edited_image = ImageEnhance.Contrast(edited_image).enhance(contrast)

col1, col2 = st.columns(2)
with col1:
    st.subheader("Original Image")
    st.image(image, use_container_width=True)

with col2:
    st.subheader("Edited Image")
    st.image(edited_image, use_container_width=True)
