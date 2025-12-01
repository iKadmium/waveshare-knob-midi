<script lang="ts">
	import type { CcAttribute } from '$lib/models/midi-attribute';

	interface CcAttributeEditorProps {
		attribute: CcAttribute;
		onRemove?: () => unknown;
	}

	let { attribute = $bindable(), onRemove }: CcAttributeEditorProps = $props();
	function handleTitleChange(event: Event) {
		const target = event.target as HTMLInputElement;
		const updated = { ...attribute, title: target.value };
		attribute = updated;
	}

	function handleChannelChange(event: Event) {
		const target = event.target as HTMLInputElement;
		const channel = parseInt(target.value, 10);
		if (!isNaN(channel) && channel >= 1 && channel <= 16) {
			const updated = { ...attribute, channel };
			attribute = updated;
		}
	}

	function handleCcChange(event: Event) {
		const target = event.target as HTMLInputElement;
		const cc = parseInt(target.value, 10);
		if (!isNaN(cc) && cc >= 0 && cc <= 127) {
			const updated = { ...attribute, cc };
			attribute = updated;
		}
	}
</script>

<article>
	<header>
		<div class="header-content">
			<h3>CC</h3>

			<button type="button" class="secondary" onclick={onRemove}>🚫</button>
		</div>
	</header>

	<form>
		<div class="grid">
			<label>
				Title
				<input
					type="text"
					value={attribute.title}
					oninput={handleTitleChange}
					placeholder="Enter title"
				/>
			</label>

			<label>
				MIDI Channel (1-16)
				<input
					type="number"
					value={attribute.channel}
					oninput={handleChannelChange}
					min="1"
					max="16"
					placeholder="1"
				/>
			</label>

			<label>
				CC Number (0-127)
				<input
					type="number"
					value={attribute.cc}
					oninput={handleCcChange}
					min="0"
					max="127"
					placeholder="0"
				/>
			</label>
		</div>
	</form>
</article>
